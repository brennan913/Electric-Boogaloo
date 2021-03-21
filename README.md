# Electric Boogaloo
A string based language for representing musical notation using only ASCII characters. Created as a group project for Bjarne Stroustrup's "Design Using C++" course (COMS 4995) by Brennan Xavier McManus, Conder Shou, and Dean Deng. Implemented as an expansion of the C++11 MIDI-format parsing library [midifile](https://midifile.sapp.org/). A presentation summarizing the content below can be found [here](https://docs.google.com/presentation/d/1EE-DDxcoUErdAO15qPXVpA72CugJSXEge9kujGQXx34/edit?usp=sharing). 

# Representing Music 
Electric Boogaloo represents real musical concepts programmatically: program with `Notes` and `Keys`, not unsigned integers. It also provides a flexible, string-based input language for the representation of music. The following is an example of how to write hot cross buns, written using each of Electric Boogaloo, the original Midifile, and MidiWriterJS.

Electric boogaloo:
```cpp
#include "MidiOutput.hpp"

using namespace smf;

int main() {

    Track melody{"E D C . E D C . C C D D E D C ."};
    MidiOutput out{ melody };
    out.write("hot_cross_buns.mid");

    return 0;
}
```

Midifile:
```cpp
idiFile midifile;
   int track   = 0;
   int channel = 0;

   int tpq     = midifile.getTPQ();
   int count   = options.getInteger("note-count");
   for (int i=0; i<count; i++) {
      int starttick = int(starttime(i) / 4.0 * tpq);
      int key       = pitch(i); // Get the next key [64, 62, 60, 64, 62, 60 …] 
      int endtick   = starttick + int(duration(mt) / 4.0 * tpq);
      midifile.addNoteOn (track, starttick, channel, key, velocity(mt));
      midifile.addNoteOff(track, endtick,   channel, key);
   }
   midifile.sortTracks();  // Need to sort tracks since added events are
                           // appended to track in random tick order.
   string filename = options.getString("output-file");
   midifile.write(filename);
```
MidiWriterJS:
```js
var MidiWriter = require('midi-writer-js');

var track = new MidiWriter.Track();

track.addEvent([
			new MidiWriter.NoteEvent({pitch: ['E4','D4'], duration: '4'}),
			new MidiWriter.NoteEvent({pitch: ['C4'], duration: '2'}),
			new MidiWriter.NoteEvent({pitch: ['E4','D4'], duration: '4'}),
			new MidiWriter.NoteEvent({pitch: ['C4'], duration: '2'}),
			new MidiWriter.NoteEvent({pitch: ['C4', 'C4', 'C4', 'C4', 'D4', 'D4', 'D4', 'D4'], duration: '8'}),
			new MidiWriter.NoteEvent({pitch: ['E4','D4'], duration: '4'}),
			new MidiWriter.NoteEvent({pitch: ['C4'], duration: '2'})
	], function(event, index) {
    return {sequential:true};
  }
);
var write = new MidiWriter.Writer(track);
console.log(write.dataUri());

```

# Central Constructs 
(TODO)

# Added Content

## cs4995-library
Contains class structure and logic for representing musical notes, and conversions between these structures and their string representation. 
## cs4995-include
Contains headers and type definitions for the library. 
## cs4995-programs
Contains example programs that demonstrate the use of the string-based input language to represent musical ideas. 
## Makefile.cs4995
Builds the programs in ./cs4995-programs.
