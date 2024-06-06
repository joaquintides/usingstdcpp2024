using std::cpp 2024
-------------------

Presentation from Joaquín Mª López Muñoz at [using std::cpp 2024](https://eventos.uc3m.es/105614/detail/using-std-cpp-2024.html)
and associated material.

<p align="center"><a href="https://youtu.be/yOo6GnbKzp8" target="_blank">
 <img src="https://img.youtube.com/vi/yOo6GnbKzp8/mqdefault.jpg" alt="Presentation video" width="320" height="180"/>
</a></p>

[Perfect Hashing in an Imperfect World](https://github.com/joaquintides/usingstdcpp2024/raw/main/Perfect%20hashing%20in%20an%20imperfect%20world.pdf):
Unlike regular hash functions, so-called perfect hash functions guarantee that no collisions ever happen,
that is, every two distinct keys map to different hash values, which allows for the construction of
hash tables with strict _O_(1) performance. This seemingly impossible feat comes with the tradeoff that
the set of elements must be known in advance prior to table initialization. In this talk we'll review
the basics of perfect hashing theory, explain some of the most common algorithms found in the literature,
review some C++ perfect hashing libraries and learn how perfect hashing can be used to improve
the efficiency of our programs.
