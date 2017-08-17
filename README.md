# NIM - PS2 Edition

### What is it?
*Nim is a mathematical game of strategy in which two players take turns removing objects from distinct heaps.
On each turn, a player must remove at least one object, and may remove any number of objects provided they all
come from the same heap. The goal of the game is to avoid being the player who must remove the last object.* -- Wikipedia

<p align="center">
	<img align="center" src="https://raw.githubusercontent.com/Theldus/NIM/master/images/nim.png" alt="NIM Game
	running in PCSX2">
	<br>
	<i>NIM game running in PCSX2</i>
</p>


### Why?
Because it's cool? Of course, that's not all. Well, I have always tried to create software for the maximum number
of architectures I could get and to develop for the current consoles is very trivial and it is possible to find a
lot of good content through the internet. Also, it was a very interesting experience and I learned a lot from it =).

Developing for PS2 was a big challenge for me, there are a dozen things I did not know and others that I still don't
know, the architecture is completely different from an Intel x86 and there are lots of extra things that you need to
take into account.

If you think that creating games for PS2 consists of dragging cute colored buttons from side to side through an
engine using some kawaii programming language, give up, it's not for you.

### How?
AFAIK, there is only one 'official' form of development for PS2 that is using the official Sony SDK that costs a
few tons of moneys.

Fortunately, a very clever group -- [ps2dev](https://github.com/ps2dev) -- was able through Sony's processor
manuals to create a complete game development environment for PlayStation 2, so thanks to them, we can now
develop for PS2 =).

So to compile this code and/or develop your games you need to download
[ps2toolchain](https://github.com/ps2dev/ps2toolchain)* and know C or C++.

*I highly recommend you to checkout for the commit `101e8f857bf26a4a174a3c67a0619beb6c5d5564`, since it was this
version of the toolchain I used and I cannot guarantee that the code will work in other versions.

### Works on real hardware?
Of course, but only for non-original consoles, that is, consoles that were unlocked to play pirate games. So
you're encouraging piracy? Of course not, and I always encourage the purchase of original games, but if you
are (or know) some unhappy owner, you can test it there. Why pirate? The chip inserted into the pirated
consoles adds some extra features to the console, such as running a boot file, which opens up space for a
plenty of new features.

You will also always be free to test in a completely legal way using an emulator such as the well known
[PCSX2](https://pcsx2.net/).

### Notes
- I want to add to this repository, in my free time, the support materials that I have used, as well as links
that helped me a lot to accomplish that.

- Also I plan to document all my code, as a tutorial for beginners.

- Do not expect my code to be written in the best possible way and error-free, I am just as beginner as
(possibly) anyone who is reading this, so if you find errors, have suggestions, or want to contribute in
any way, let me know.
