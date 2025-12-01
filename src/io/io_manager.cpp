/*

COMMS FORMAT

All messages follow this format:

[\n]FIRE[\t]7iNYUnEq8Mo4FcdHmpSH3A[\t]ip7d
    ^^^^    ^^^^^^^^^^^^^^^^^^^^^^    ^^^^
     |       |                         |
     |       |                        4-digit checksum of data in ASCII->base64.
     |       |
     |      Message data in ASCII->base64.
     |
   Type of message being delivered.


Types of messages:
    computer -> teensy:
    PWRON   nEq8   nEq8
    PWROFF  cdHm   cdHm
    FIRE    iNYU   iNYU
    ABORT   pSH3   pSH3
    PURGE   4Fcd   4Fcd
        For all 5 of these, the message is a random string.
        This command will send on repeat to the Teensy
        until the Teensy correctly returns the same command,
        as a receipt.

    teensy -> computer:
    DATA 61vBYGG1lRztZaOSIsYZxXcuYW8fzE/z hJ7d
        The message is a tsv containing rows of telemetry data.
    ERROR y5PBMkFGOp f7Tb
        The message is a block of text describing the error.



*/