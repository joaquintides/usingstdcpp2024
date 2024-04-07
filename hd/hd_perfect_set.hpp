/* PoC of an HD(C)-based perfect set.
 * https://cmph.sourceforge.net/papers/esa09.pdf
 *
 * Copyright 2023 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef HD_PERFECT_SET_HPP
#define HD_PERFECT_SET_HPP

#include <algorithm>
#include <boost/config.hpp>
#include <boost/container_hash/hash.hpp>
#include <boost/core/bit.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/unordered/detail/mulx.hpp>
#include <boost/unordered/detail/xmx.hpp>
#include <climits>
#include <numeric>
#include <utility>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
#include "mulxp_hash.hpp"

#ifdef HD_PERFECT_SET_TRACE
#include <iostream>
#endif

namespace hd{

struct construction_failure:std::runtime_error
{
  construction_failure():
    std::runtime_error("could not construct the container"){}
};

struct duplicate_element:std::runtime_error
{
  duplicate_element():
    std::runtime_error("duplicate elements found"){}
};

struct duplicate_hash:std::runtime_error
{
  duplicate_hash():
    std::runtime_error("duplicate hash values found"){}
};

struct pow2_lower_size_policy
{
  static constexpr inline std::size_t size_index(std::size_t n)
  {
    auto exp=n<=2?1:((std::size_t)(boost::core::bit_width(n-1)));
    return (std::size_t(1)<<exp)-1;
  }

  static constexpr  inline std::size_t size(std::size_t size_index_)
  {
     return size_index_+1;
  }
    
  static constexpr std::size_t min_size(){return 2;}

  static constexpr inline std::size_t position(std::size_t hash,std::size_t size_index_)
  {
    return hash&size_index_;
  }
};

struct pow2_upper_size_policy
{
  static constexpr inline std::size_t size_index(std::size_t n)
  {
    return sizeof(std::size_t)*CHAR_BIT-
      (n<=2?1:((std::size_t)(boost::core::bit_width(n-1))));
  }

  static constexpr inline std::size_t size(std::size_t size_index_)
  {
     return std::size_t(1)<<(sizeof(std::size_t)*CHAR_BIT-size_index_);  
  }
    
  static constexpr std::size_t min_size(){return 2;}

  static constexpr inline std::size_t position(std::size_t hash,std::size_t size_index_)
  {
    return hash>>size_index_;
  }
};

template<
  typename T,typename Hash=boost::hash<T>,typename Pred=std::equal_to<T>
>
class perfect_set
{
  using element_array=std::vector<T>;
  using displacement_size_policy=pow2_lower_size_policy;
  using element_size_policy=pow2_upper_size_policy;

public:
  static constexpr std::size_t default_lambda=4;
  using key_type=T;
  using value_type=T;
  using hasher=Hash;
  using key_equal=Pred;
  using iterator=typename element_array::const_iterator;

  template<typename FwdIterator>
  perfect_set(
    FwdIterator first,FwdIterator last,std::size_t lambda=default_lambda)
  {
    while(lambda){
      if(construct(first,last,lambda))return;
      lambda/=2;
    }
    throw construction_failure{};
  }

  iterator begin()const{return elements.begin();}
  iterator end()const{return elements.begin()+size_;}

  template<typename Key>
  BOOST_FORCEINLINE iterator find(const Key& x)const
  {
    auto hash=h(x);
    auto pos=element_position(hash,displacements[displacement_position(hash)]);
    if(pos>=size_||!pred(x,elements[pos]))pos=size_;
    return elements.begin()+pos;
  }

private:
  using displacement_info=std::pair<std::size_t,std::size_t>;
  template<typename FwdIterator>
  struct bucket_node
  {
    FwdIterator  it;
    std::size_t  hash;
    bucket_node *next=nullptr;
  };
  template<typename FwdIterator>
  struct bucket_entry
  {
    bucket_node<FwdIterator> *begin=nullptr;
    std::size_t               size=0;
  };

  template<typename FwdIterator>
  bool construct(FwdIterator first,FwdIterator last,std::size_t lambda)
  {
    using bucket_node_array=std::vector<bucket_node<FwdIterator>>;
    using bucket_array=std::vector<bucket_entry<FwdIterator>>;

    size_=static_cast<std::size_t>(std::distance(first,last));
    dsize_index=displacement_size_policy::size_index(size_/lambda);
    displacements.resize(displacement_size_policy::size(dsize_index));
    displacements.shrink_to_fit();

    /* extended_size is a power of two strictly no smaller than the element
     * array size. Construction and lookup work as if with a virtual extended
     * array whose positions from size_ are taken up. 
     */

    size_index=element_size_policy::size_index(size_);
    auto extended_size=element_size_policy::size(size_index);
    elements.resize(size_);
    elements.shrink_to_fit();

    bucket_node_array bucket_nodes;
    bucket_array      buckets(displacements.size());
    bucket_nodes.reserve(size_);
    for(auto it=first;it!=last;++it){
      auto   hash=h(*it);
      auto  &root=buckets[displacement_position(hash)];
      auto **ppnode=&root.begin;
      while(*ppnode){
        if((*ppnode)->hash==hash){
          if(pred(*((*ppnode)->it),*(it)))throw duplicate_element{};
          else                            throw duplicate_hash{};
        }
        ppnode=&(*ppnode)->next;
      }
      bucket_nodes.push_back({it,hash});
      *ppnode=&bucket_nodes.back();
      ++root.size;
    }

    std::vector<std::size_t> sorted_bucket_indices(buckets.size());
    std::iota(sorted_bucket_indices.begin(),sorted_bucket_indices.end(),0u);
    std::sort(
      sorted_bucket_indices.begin(),sorted_bucket_indices.end(),
      [&](std::size_t i1,std::size_t i2){
        return buckets[i1].size>buckets[i2].size;
      });

    boost::dynamic_bitset<>  mask;
    mask.resize(size_,true); /* true --> available */
    std::vector<std::size_t> bucket_positions;
#ifdef HD_PERFECT_SET_TRACE
    std::size_t num_inserted=0;
#endif

#if 1
    std::size_t i=0;
    for(;i<buckets.size();++i){
      const auto& bucket=buckets[sorted_bucket_indices[i]];
      if(bucket.size<=1)break; /* on to buckets of size 1 */

#ifdef HD_PERFECT_SET_TRACE
      if(i%10000==0)std::cout<<i<<":\t"<<bucket.size<<"\t"<<num_inserted<<"\n";
      num_inserted+=bucket.size;
#endif

      for(std::size_t d0=0;d0<extended_size;++d0){
        for(std::size_t d1=0;d1<extended_size;++d1){
          /* this calculation critically depends on displacement_size_policy */
          displacement_info d={d0<<size_index,(d1<<32)+1};

          bucket_positions.clear();
          for(auto pnode=bucket.begin;pnode;pnode=pnode->next){
            auto pos=element_position(pnode->hash,d);
            if(pos>=size_||!mask[pos]){
              for(auto pos2:bucket_positions)mask[pos2]=true;
              goto next_displacement;
            }
            mask[pos]=false;
            bucket_positions.push_back(pos);
          }
          displacements[sorted_bucket_indices[i]]=d;
          {
            auto pnode=bucket.begin;
            for(auto pos:bucket_positions){
              elements[pos]=*(pnode->it);
              pnode=pnode->next;
            }
          }
          goto next_bucket;
          next_displacement:;
        }
      }
      return false;
    next_bucket:;
    }
#else
    std::vector<std::size_t> bucket_muls;
    std::size_t i=0;
    for(;i<buckets.size();++i){
      const auto& bucket=buckets[sorted_bucket_indices[i]];
      if(bucket.size<=1)break; /* on to buckets of size 1 */

#ifdef HD_PERFECT_SET_TRACE
      if(i%10000==0)std::cout<<i<<":\t"<<bucket.size<<"\t"<<num_inserted<<"\n";
      num_inserted+=bucket.size;
#endif

      for(std::size_t d1=0;d1<extended_size;++d1){
        displacement_info d={0,(d1<<32)+1};
        bucket_muls.clear();
        for(auto pnode=bucket.begin;pnode;pnode=pnode->next){
          bucket_muls.push_back(pnode->hash*d.second);
        }

        for(auto d0=mask.find_first();d0<size_;d0=mask.find_next(d0)){
          d.first=(d0-bucket_muls[0])<<size_index;
          bucket_positions.clear();
          for(auto mul:bucket_muls){
            auto pos=element_size_policy::position(d.first+mul,size_index);
            if(pos>=size_||!mask[pos]||
               std::find(
                 bucket_positions.begin(),
                 bucket_positions.end(),pos)!=bucket_positions.end()){
              goto next_displacement;
            }
            bucket_positions.push_back(pos);
          }
          displacements[sorted_bucket_indices[i]]=d;
          for(auto pnode=bucket.begin;pnode;pnode=pnode->next){
            auto pos=element_position(pnode->hash,d);
            elements[pos]=*(pnode->it);
            mask[pos]=false;
          }
          goto next_bucket;
          next_displacement:;
        }
      }
      return false;
    next_bucket:;
    }
#endif

    /* buckets of size <=1 */

    auto pos=mask.find_first();
    for(;i<buckets.size();++i){
      const auto& bucket=buckets[sorted_bucket_indices[i]];
      if(!bucket.size)break; /* remaining buckets also empty */

#ifdef HD_PERFECT_SET_TRACE
      if(i%10000==0)std::cout<<i<<":\t"<<bucket.size<<"\t"<<num_inserted<<"\n";
      num_inserted+=bucket.size;
#endif
      /* this calculation critically depends on displacement_size_policy */
      displacements[sorted_bucket_indices[i]]={pos<<size_index,0};
      elements[pos]=*(bucket.begin->it);
      mask[pos]=false;
      pos=mask.find_next(pos);
    }

    for(;i<buckets.size();++i){
      /* send all empty buckets off range */
      displacements[sorted_bucket_indices[i]]={~std::size_t(0),0};
    }

    return true;
  }

  std::size_t displacement_position(std::size_t hash)const
  {
    return displacement_size_policy::position(hash,dsize_index);
  }

  std::size_t element_position(
    std::size_t hash,const displacement_info& d)const
  {
    return element_size_policy::position(d.first+d.second*hash,size_index);
  }

  hasher                         h;
  key_equal                      pred;
  std::size_t                    size_;
  std::size_t                    dsize_index;
  std::vector<displacement_info> displacements;
  std::size_t                    size_index;
  element_array                  elements;
};

/* some mixers */

struct mulx_hash
{
  std::size_t operator()(std::size_t x)const
  {
    return boost::unordered::detail::mulx(x);
  }
};

struct xmx_hash
{
  std::size_t operator()(std::size_t x)const
  {
    return boost::unordered::detail::xmx(x);
  }
};

struct xm_hash
{
  std::size_t constexpr operator()(std::size_t x)const
  {
    x ^= x >> 23;
    x *= 0xff51afd7ed558ccdull;

    return x;
  }
};

struct m_hash
{
  std::size_t constexpr operator()(std::size_t x)const
  {
    x *= 0xff51afd7ed558ccdull;
    return x;
  }
};

struct mbs_hash
{
  std::size_t constexpr operator()(std::size_t x)const
  {
    x *= 0xff51afd7ed558ccdull;
    return boost::core::byteswap(x);
  }
};

constexpr inline std::uint64_t read64le( char const * p )
{
    if( std::is_constant_evaluated() )
    {
      std::uint64_t w =
          static_cast<std::uint64_t>( static_cast<unsigned char>( p[0] ) ) |
          static_cast<std::uint64_t>( static_cast<unsigned char>( p[1] ) ) <<  8 |
          static_cast<std::uint64_t>( static_cast<unsigned char>( p[2] ) ) << 16 |
          static_cast<std::uint64_t>( static_cast<unsigned char>( p[3] ) ) << 24 |
          static_cast<std::uint64_t>( static_cast<unsigned char>( p[4] ) ) << 32 |
          static_cast<std::uint64_t>( static_cast<unsigned char>( p[5] ) ) << 40 |
          static_cast<std::uint64_t>( static_cast<unsigned char>( p[6] ) ) << 48 |
          static_cast<std::uint64_t>( static_cast<unsigned char>( p[7] ) ) << 56;

      return w;
    }
    else
    {
      return ::read64le( reinterpret_cast<unsigned char const *>(p) );
    }
}

constexpr inline std::uint32_t read32le( char const* p )
{
    if( std::is_constant_evaluated() )
    {
      std::uint32_t w =
          static_cast<std::uint32_t>( static_cast<unsigned char>( p[0] ) ) |
          static_cast<std::uint32_t>( static_cast<unsigned char>( p[1] ) ) <<  8 |
          static_cast<std::uint32_t>( static_cast<unsigned char>( p[2] ) ) << 16 |
          static_cast<std::uint32_t>( static_cast<unsigned char>( p[3] ) ) << 24;

      return w;
    }
    else
    {
      return ::read32le( reinterpret_cast<unsigned char const *>(p) );
    }
}

struct mulxp3_string_hash
{
  template<typename String>
  constexpr inline std::uint64_t operator()(const String& x,std::uint64_t seed = 0) const
  {
      char const * p = x.data();
      std::size_t n = x.size();

      std::uint64_t const q = 0x9e3779b97f4a7c15ULL;
      std::uint64_t const k = q * q;

      std::uint64_t w = seed;
      std::uint64_t h = w ^ n;

      while( n >= 16 )
      {
          std::uint64_t v1 = read64le( p + 0 );
          std::uint64_t v2 = read64le( p + 8 );

          w += q;
          h ^= mulx( v1 + w, v2 + w + k );

          p += 16;
          n -= 16;
      }

      {
          std::uint64_t v1 = 0;
          std::uint64_t v2 = 0;

          if( n > 8 )
          {
              v1 = read64le( p );
              v2 = read64le( p + n - 8 ) >> ( 16 - n ) * 8;
          }
          else if( n >= 4 )
          {
              v1 = (std::uint64_t)read32le( p + n - 4 ) << ( n - 4 ) * 8 | read32le( p );
          }
          else if( n >= 1 )
          {
              std::size_t const x1 = ( n - 1 ) & 2; // 1: 0, 2: 0, 3: 2
              std::size_t const x2 = n >> 1;        // 1: 0, 2: 1, 3: 1

              v1 = (std::uint64_t)static_cast<unsigned char>(p[ x1 ]) << x1 * 8 | 
                   (std::uint64_t)static_cast<unsigned char>(p[ x2 ]) << x2 * 8 | 
                   (std::uint64_t)static_cast<unsigned char>(p[ 0 ]);
          }

          w += q;
          h ^= mulx( v1 + w, v2 + w + k );
      }

      return h;
  }
};

} /* namespace hd */

#endif

