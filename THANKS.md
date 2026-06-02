# THANKS

This project benefits from the contributions and feedback of many individuals.

We would like to especially thank:

*   **Felipe Gasper** ([@felipegasper](https://github.com/felipegasper)) - For crucial feedback on Perl XS best practices, particularly regarding the safe handling of Perl strings with `SvPVbyte` and `SvPVutf8`.

*   **Dean Hamstead** ([@djzort](https://github.com/djzort)) - For publicity, encouragement, valuable build system feedback (identifying issues with missing `cpanfile` and `Makefile.PL` dependencies), and testing the `README.md` instructions on different environments, which led to significant improvements in the build process.

*   **Leon Timmermans** ([@Leont](https://github.com/Leont)) - For expressing interest and willingness to contribute.

*   **Brad Fitzpatrick** ([@bradfitz](https://github.com/bradfitz)) - For the original 2008 `protobuf-perlxs` implementation. His comprehensive test suite and API design goals for high-performance Protocol Buffers in Perl continue to serve as the benchmark and foundation for this modern, UPB-based extension.

*   **Mattia Barbon** ([@mbarbon](https://github.com/mbarbon)) - For his work on `Google::ProtocolBuffers::Dynamic`. His implementation provided a vital performance target and architectural reference during the development of our "Fast Accessor" XSUB optimization layer.

*   **Sergey Stryapukhin** ([@sergey-stryapukhin](https://github.com/sergey-stryapukhin)) - For maintaining the pure-Perl `Google::ProtocolBuffers` implementation, which served as an essential baseline for performance benchmarking and API compatibility.

*   The **p5p community** - For their engagement and suggestions.

*   The authors and maintainers of the **upb** library.

Your contributions help make this project better!
