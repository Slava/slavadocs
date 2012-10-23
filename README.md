# Slava Docs

Simple server-client application will establish connection via sockets to server. Server should make web page refreshable. It should be written in pure C/C++ without additional libraries or frameworks.

## Purpose
Purpose of this useless program - learn low-level socket usage. Second purpose was to learn C better (also Makefile system and separate compilation to object files, etc). Third purpose was to get idea of GTK+ lib.

## Install
All you need is GTK+-3.0 libs and make utility. Just `make all` and it will compile, hopefully.

## What is inside
`server <port> <pathtofile> <pathtofolder>` -- runs on server side (C.O.), parameters: listen port, output page, root of HTDOCS. Creates output page, that auto loads `slavadocs-data.txt` file from root of HTDOCS. Not even funny.

`client <host> <port> [kill]` -- cli client. You can only send line by line, ending with EOF. `kill` option send terminate signal to server application. Sad, but it is the only way to stop server application _normally_.

`client_gui` -- gui client. Written using GTK+-3.0 libs, which gives you ability to edit text, after each edit, application will resend whole text. Yes, it is stupid. What did you want from me?

## In the end
It was written in two and half night. Do not judge me hard, first attempt to be a *real programmer*, not just competitive one. 
