Robot
=====

Robot is a toy evented TCP/IP server written in C. You can send it commands like MOVE or LEFT to find out the secret hidden in the middle of the town. It was written for the sake of education.

It also comes with a demo client which solves the puzzle for you. (It's pretty dumb, though.)

Compilation
-----------
    make

Server
------

To run the server just hit:

    ./robot PORT

Commands
--------

<table>
  <thead>
    <tr>
      <th>Command</th><th>Response codes</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>MOVE</td><td>240, 530, 580, 572</td>
    </tr>
    <tr>
      <td>LEFT</td><td>240</td>
    </tr>
    <tr>
      <td>LIFT</td><td>260, 550</td>
    </tr>
    <tr>
      <td>REPAIR</td><td>240, 571</td>
    </tr>
  </tbody>
</table>

Client
------

To run the client hit:

    ./robot HOSTNAME PORT

License
-------

Copyright (c) 2013 Jakub Svehla

MIT License

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
