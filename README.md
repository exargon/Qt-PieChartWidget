# Qt-PieChartWidget
This is a small project to make a dividerslider for use with qt-projects.

The goal is to have a slider with an arbitrary amount of movable dividers to divide up a defined total.

The project consists of an abstractdividerslider, which implements the internal dividing logic, slots and signaling, 
and a piechartslider, which is a visual implementation of the concept with support for input from the mouse.

Making a PieChartSlider that behaves in an intuitive way turns out to be more complex than one would think, 
and there is a lot nuance in the way it could behave. Currently I have hard-coded a behaviour I find reasonable sensible
(at least compared to complexity of the code).

### Possible future improvements:

- Improve handling of the scroll wheel (symmetric resizing and consistent speed)
- Make the divider between zero and total movable
- Add more configuration options(ways for it to behave)
- Add an untracked handle option (value does not change before handle is released)
- Visualizable string labels for each sector

### Known Bugs:
- Sectorhandles at 0 and total overlap wih eachother instead of stacking.
