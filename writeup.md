# Writeup

My app was supposed to be NFCing two devices together to exchange secret keys that allow encrypted/signed communication in the future. For time, I reduced my scope to just a global chatroom where everyone is identified by their key fingerprint (see video).

The cryptography is all done in the NDK using the OpenSSL library. The most advanced function is a multi-threaded multiple-signature verification that is done to populate previous messages when the app first opens. Two threads are kicked off to do the processing. If either thread hits an invalid signature, it tells the other thread to abort. It does this through a mutexed variable that both threads check on every loop iteration.

There are also several NDK methods exposed to Java that my application makes use of for key generation and encrypting messages.

The back-end is done with Firebase to share messages with other people. This was my first time using Firebase, and it was absolutely incredible. I was going to do what I thought was a basic Flask and Python app for storing messages, but Firebase made things so easy that I barely had to think about the back-end. And there's even a fantastic visualization/editor online for all the data, which let me quickly change one byte of a signature and make sure failures were handled correctly. I can see why Google acquired them.

I also got more experience with Android. This was probably one of the more complex things I've done. It's interesting that the NDK can be used with all the C stuff we learned in class, although the initial configuration was a huge pain, and conversion between Java and C types is tiring and error-prone. I also learned a little bit more about Android UI. ListViews make more sense to me.

See journal for more thoroughness on the issues I ran across.
