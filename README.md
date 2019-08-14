# Qt-PieChartWidget
This is a small project to make a dividerslider for use with qt-projects.

### What is a dividerslider

A dividerslider is a slider that is used to divide a given number, the total if you will, into a set of smaller numbers
through user interaction. The key difference from a normal slider is that it allows an arbirtary number of dividers instead
of just one. In addition to this it has signals and slots for both the values of the dividers and for the sectors between them.

### What it this project

This is just a small project I made for fun to explore the concept of a dividerslider described above.
The project consists of an abstractdividerslider, which implements the internal dividing logic, slots and signaling, 
and so on. It is meant to be a common base for different visual representations of a dividerslider. For now there
is only a piechartslider implementation, so the abstract base only function as a seperation of logic.

Making a PieChartSlider that behaves in an intuitive way turns out to be more complex than one would think, 
and there is a lot nuance in the way it could behave. Currently I have hard-coded a behaviour I find reasonable
(at least compared to complexity of implementatioin).

### Possible future improvements:

- Improve handling of the scroll wheel (symmetric resizing)
- Make the divider between zero and total movable
- Add more configuration options(ways for it to behave)
- Add an untracked handle option (value does not change before handle is released)
- Visualizable string labels for each sector

### Known Bugs:
- Sectorhandles at 0 and total overlap with eachother instead of stacking.
