# iptables-cfg
C program to configure iptables, using an NCURSES interface.

## Installation

Obviously, your system must use iptables to work with this program.

1.  Install ncurses
    * Debian / Ubuntu: `apt install ncurses`
    * Fedora: `dnf install ncurses`
2.  (Optional) Install netfilter-persistent to save rules across reboots
    * Debian / Ubuntu: `apt install netfilter-persistent`
    * Fedora: Coming soon...
3.  Install iptables-cfg
    * `make install`

## Usage

iptables-cfg is installed into `/usr/local/bin` by default, so simply running
`iptables-cfg` in a terminal will start the program.
