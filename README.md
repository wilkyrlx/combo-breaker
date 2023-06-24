# Combo Breaker
This project automates the Masterlock attack developed by Samy Kamkar that allows lock combinations to be brute forced in 8 attempts or less. The attack is based on a manufacturing flaw that allows an attacker to mathematically calculate 8-16 potential combinations based off of two locking positions and one resistant position. The attack is outlined on [Samy's website](https://samy.pl/master/master.html) and described in detail [on his youtube channel](https://www.youtube.com/watch?v=qkolWO6pAL8). Of course, this attack should only be attempted on locks that you own or have permission to attack.

I wanted to automate the attack to speed up the process. An arduino calculates the 8-16 potential combinations based off of the two locking positions and one resistant position. Then, a stepper motor tries each of these combinations rapidly.

## Usage

## BOM
TODO: export BOM from easyEDA
| Qty | ID| Description |
| --- | --- | --- |
| 1 |  | A4988 Stepper Motor Driver |
| 1 |  | Arduino Uno |

## Electronic Schematics

## Assembly