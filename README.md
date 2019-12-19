![alt text](https://bitbucket.org/unitxtra/cppsrtframingexample/raw/44dbc90c2f53ca12977dcfeffa4d8875f5280a02/efpsrt.png)

# SRT + EFP Client/Server example

This example uses EFP ([ElasticFramingProtocol](https://bitbucket.org/unitxtra/efp/src/master/)) as a layer between the producer/consumer of data and the transport layer. As network protocol/transport layer SRT ([SecureReliableTransport](https://github.com/Haivision/srt)) is used.


## Installation

In this example the libraries are pre-built for MacOS and Linux
(release builds but debug logging on)

## Usage

1. Open a terminal window and run `./cppSRTFramingServer`
2. Open two or more other terminal windows and run `./cppSRTFramingServer`

 