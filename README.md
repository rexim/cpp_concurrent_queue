# Concurrent Queue in C++

Simple Concurrent Queue implementation in C++ using pthreads. It is
actually implemented in C style. The only used C++ features are
- templates for generalization,
- member functions for conciseness of the implementation.

The implementation was born during research about parallel downloading
for [Vodus] project. After the research I came to the conclusion that
I don't need this queue in the project, but I didn't want to throw the
implementation away, so I put it here for future needs.

[Vodus]: https://github.com/tsoding/vodus
