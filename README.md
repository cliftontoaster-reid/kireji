# Kireji

Kireji (Japanese: 切れ字) is a C++ library with the goal of providing a feature-rich HTTP parser, for the use in web servers, proxies, and other applications that need to handle HTTP requests and responses.
It will be written in C++98, without any dependencies as well as C++ containers, its goal is to be used in restricted environments, projects where the allowed features of C++ are very limited, and to be as portable as possible.

Made for usage in 42 school projects, it will be designed to follow requirements of the `webserv` version $24.0$ project.
I am still providing this library under the MIT licence, so that it may be used to further anyone's learning experience with C++ and the HTTP protocol.

## Planned features

- HTTP request deserialization/serialization
  - HTTP/1.0
  - HTTP/1.1
  - HTTP/2.0
  - HTTP/3.0
- HTTP response deserialization/serialization
  - HTTP/1.0
  - HTTP/1.1
  - HTTP/2.0
  - HTTP/3.0
- Support for chunked transfer encoding
- Support for content compression

And others that I may find useful to implement, but the above are the main features I plan to implement in this library.

## Documentation

Will be written using Doxygen, documentation based on the code's comments, which will give the user an in-depth look at the library's API, and how to use it.
The documentation will be generated in HTML format, and will be available in the `docs` directory of the repository.

Another documentation will be written using the tool Writerside by JetBrains.
Its goal will be to provide a more user-friendly documentation, with examples, explanations, and guides on how to properly use the library.
The Writerside documentation will be available in the `docs/writerside` directory of the repository, and will be generated in HTML format as well.

When useful and relevant, the documentation will automatically be built and published to GitHub Pages, so that it can be easily accessed by anyone interested in using the library.
But as of now, it being completely empty, I will not set up the GitHub Pages deployment, until I have at least some documentation to publish.
