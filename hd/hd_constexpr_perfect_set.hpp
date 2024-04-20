/* PoC of an HD(C)-based perfect set.
 * https://cmph.sourceforge.net/papers/esa09.pdf
 *
 * Copyright 2023-2024 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef HD_CONSTEXPR_PERFECT_SET_HPP
#define HD_CONSTEXPR_PERFECT_SET_HPP

#include <algorithm>
#include <array>
#include <boost/config.hpp>
#include <boost/container_hash/hash.hpp>
#include <boost/core/bit.hpp>
#include <boost/dynamic_bitset.hpp>
#include <climits>
#include <initializer_list>
#include <numeric>
#include <utility>
#include <stdexcept>
#include <string>
#include <vector>
#include "hd_perfect_set.hpp"

namespace hd{

template<
  typename T,std::size_t N,
  typename Hash=boost::hash<T>,typename Pred=std::equal_to<T>
>
class constexpr_perfect_set
{
public:
  static constexpr std::size_t lambda=2;

private:
  using displacement_size_policy=pow2_lower_size_policy;
  using element_size_policy=pow2_upper_size_policy;
  static constexpr std::size_t dsize_index=
    displacement_size_policy::size_index(N/lambda);
  using displacement_info=std::pair<std::size_t,std::size_t>;
  using displacement_array=std::array<
    displacement_info,
    displacement_size_policy::size(dsize_index)>;
  using element_array=std::array<T,N>;

public:
  using key_type=T;
  using value_type=T;
  using hasher=Hash;
  using key_equal=Pred;
  using iterator=typename element_array::const_iterator;

  template<typename Value>
  constexpr constexpr_perfect_set(Value const (&a)[N])
  {
    construct(&a[0],&a[N]);
  }

  template<typename Value>
  constexpr constexpr_perfect_set(const std::array<Value,N> a)
  {
    construct(a.begin(),a.end());
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
  constexpr bool construct(FwdIterator first,FwdIterator last)
  {
    using bucket_node_array=std::vector<bucket_node<FwdIterator>>;
    using bucket_array=std::vector<bucket_entry<FwdIterator>>;

    /* extended_size is a power of two strictly no smaller than the element
     * array size. Construction and lookup work as if with a virtual extended
     * array whose positions from size_ are taken up. 
     */

    size_index=element_size_policy::size_index(size_);
    auto extended_size=element_size_policy::size(size_index);

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

    std::vector<unsigned char> mask;
    mask.resize(size_,true); /* true --> available */
    std::vector<std::size_t> bucket_positions;

    std::size_t i=0;
    for(;i<buckets.size();++i){
      bool next_bucket=false;
      const auto& bucket=buckets[sorted_bucket_indices[i]];
      if(bucket.size<=0)break; /* on to buckets of size 1 */

      for(std::size_t d0=0;d0<extended_size;++d0){
        for(std::size_t d1=0;d1<extended_size;++d1){
          /* this calculation critically depends on displacement_size_policy */
          displacement_info d={d0<<size_index,(d1<<32)+1};

          bucket_positions.clear();
          bool next_displacement=false;
          for(auto pnode=bucket.begin;pnode;pnode=pnode->next){
            auto pos=element_position(pnode->hash,d);
            if(pos>=size_||!mask[pos]){
              for(auto pos2:bucket_positions)mask[pos2]=true;
              next_displacement=true;
              break;
            }
            else{
              mask[pos]=false;
              bucket_positions.push_back(pos);
            }
          }
          if(next_displacement)continue;
          displacements[sorted_bucket_indices[i]]=d;
          auto pnode=bucket.begin;
          for(auto pos:bucket_positions){
            elements[pos]=*(pnode->it);
            pnode=pnode->next;
          }
          next_bucket=true;
          break;
        }
        if(next_bucket)break;
      }
      if(next_bucket)continue;
      throw 1;
      return false;
    }

    for(;i<buckets.size();++i){
      /* send all empty buckets off range */
      displacements[sorted_bucket_indices[i]]={~std::size_t(0),0};
    }
    return true;
  }

  std::size_t constexpr displacement_position(std::size_t hash)const
  {
    return displacement_size_policy::position(hash,dsize_index);
  }

  std::size_t constexpr element_position(
    std::size_t hash,const displacement_info& d)const
  {
    return element_size_policy::position(d.first+d.second*hash,size_index);
  }

  hasher                       h;
  key_equal                    pred;
  static constexpr std::size_t size_=N;
  displacement_array           displacements={};
  std::size_t                  size_index;
  element_array                elements={};
};

} /* namespace hd */

#endif

