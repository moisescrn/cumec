# cumec - Customisable Metronome in C

This is a command line metronome (in the future an GUI is planned to be implemented) that is fully customizable, in velocity, beats and in rhythm patterns.
There are some basic predefined beats (like 3/4, 4/4,...), but it is possible to create new ones with total freedom, even irregular beats, like those used in flamenco.



#### Commands

For the moment the commands are the following

| Command | Action                                                    |
| ------- | --------------------------------------------------------- |
| q       | quit                                                      |
| p       | toggle between pause and resume                           |
| +       | increase beats per minute (bpm) by 1                      |
| -       | decrease bpm by 1                                         |
| .       | increase bpm by 10                                        |
| ,       | decrease bpm by 10                                        |
| 1-10    | change to regular rhythm with the pressed number as beats |
