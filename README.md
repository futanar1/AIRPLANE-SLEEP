
libaribcaption
======
[日本語](README_ja.md)

A portable caption decoder / renderer for handling ARIB STD-B24 based TV broadcast captions.

## Background
While **CEA-608/708** closed caption standards are used by the **ATSC** system in North America,
**DVB Subtitles / DVB Teletext** defined in **DVB** standard are used in Europe and many parts of the world,
Japan established its own TV broadcasting standard **ISDB** that includes a kind of caption service defined in **ARIB STD-B24**
by the **A**ssociation of **R**adio **I**ndustries and **B**usinesses (ARIB).

Brazil also adopted **ISDB-T** International for their broadcasting by establishing Brazilian version **SBTVD / ISDB-Tb** based on the Japanese standard,
which has been widely used in South America countries and around the world.
Brazilian version also includes a caption service for Latin languages defined in **ABNT NBR 15606-1** which is modified from **ARIB STD-B24** specification.
Philippines also adopted ISDB-T International based on the Brazilian standards,
but uses **UTF-8** for caption encoding based on the Japansese specification **ARIB STD-B24**.

Though ISDB-based TV broadcasting has been operating for about 20 years, ARIB based caption is still lacking support in general players.

## Overview
libaribcaption provides decoder and renderer for handling ARIB STD-B24 based broadcast captions,
making it possible for general players to render ARIB captions with the same effect (or even better) as Television.

libaribcaption is written in C++17 but also provides C interfaces to make it easier to integrate into video players.
It is a lightweight library that only depends on libfreetype and libfontconfig in the worst case.

libaribcaption is a cross-platform library that works on various platforms, including but not limited to: