# serial connection between brains

Connect the brains together on port 1, and plug a cable with a motor into either brains port 20. This will be the receiver.
Connect a controller to the brain without the motor, and start the program.

# IMPORTANT
Press the **RECEIVE** button FIRST, on the Receiver brain.
Press the **SEND** button SECOND, on the Sender brain.

Move the left Y axis. The motor should follow.

Latency is a problem right now, higher baudrate might fix things but uncertain.

# why?

The whole point of this is that once you hit 8 motors on a vex brain, the current is software limited by the brain. I don't like that, and the most obvious method to fix this is add another brain.
So... this repository does that.

I know the code is spaghetti and has no instructions, but if you want to use this for anything you should be able to understand how it works.

Use the `pros terminal` to debug. It is massively useful.
