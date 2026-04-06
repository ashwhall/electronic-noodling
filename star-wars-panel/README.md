# Star Wars Panel Button Sequences

A silly little mock wall panel I designed a 3d-printed for a sci-fi themed murder mystery party.

**99% vibe-coded, because why bother for such a low-stakes project?**

It wasn't intended to be shared, but might as well put it online. Message me if you want the original Fusion 360 files or any other photos.

## How to Use

Press a sequence of colored buttons (Yellow, Blue, and/or Red), then press White to trigger the animation. You have 1 second between button presses before the sequence times out.

## Available Sequences

### Yellow + White: **Chase**

A single LED chases around the panel (Red1 → Red2 → White2 → White1) while a single segment chases around the 7-segment display perimeter. Runs for 3 seconds.

### Blue + White: **Crescendo**

All LEDs and the 7-segment display flash together with decreasing intervals, building to a rapid crescendo finale. Starts slow (800ms) and accelerates down to 50ms.

### Red + White: **Binary Count**

The 4 LEDs display binary counting from 0000 to 1111 while the 7-segment display shows the corresponding hexadecimal value (0-F). Updates every 500ms.

### Yellow, Blue + White: **Cascade**

LEDs turn on one-by-one, hold briefly with all segments lit, then turn off one-by-one. The 7-segment display fills progressively during the on phase.

### Yellow, Red + White: **BOOBS**

> Don't judge me. It's for a party and I thought it would be funny...

Displays the letters "I_LOvE_BOOBS" on the 7-segment display, holding each letter for 750ms, with the lights turned off for 250ms between letters.

### Yellow, Blue, Red + White: **Open**

Rapidly spins the border segments of the display, then lights up and keeps the white LEDs illuminated. The 7-segment display then cycles through the letters 'airlock_open', then turns off the LEDs one by one and returns to normal operation. Simulates unlocking an airlock door.

## Invalid Sequences

If you enter an invalid sequence or press White without any preceding buttons, all LEDs will flash 3 times rapidly as feedback.

## Idle Animation

When the panel has been inactive for 5 seconds, the 7-segment display shows random "alien" character patterns every second. Press any button to return to normal operation.
