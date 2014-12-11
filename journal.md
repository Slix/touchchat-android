# Oct 18

I have a few ideas for projects. I’ve done some simple games before. I’d really like to make a coop game like Spaceteam based on puzzles where your mistakes hinder your teammates.

# Nov 3

Added vague project details that I want to do something involving cryptography. That sounds easily parallelizable, and I’m interested in it.

# Nov 15

TouchChat is a good idea. Sharing public keys is very difficult, and PKI is broken in general. Using NFC to exchange information requires that you actually meet someone in real-life before you can communicate securely with them.

Added these details to the README.

# Dec 7 8:10pm

Installing NDK tooks on laptop. Latest PKGBUILD for Arch is broken, so I have to manually fix it.

# Dec 7 8:31pm

I was going to write my backend with Flask and use Google Cloud Messaging to inform the app of new messages. But I need to radically simplify this to finish in time. I’m going to use Firebase to store users’ chat streams as arrays.

Firebase arrays are somewhat odd, so I’ll need to be careful about sync race conditions, possibly by keeping two separate arrays (one for each side of the conversation).

# Dec 7 8:48pm

According to http://www.shaneenishry.com/blog/2014/08/17/ndk-with-android-studio/ , it’s possible to use the NDK with Android Studio as long as you compile the NDK component first. That should be a relatively easy workflow for me and much better than trying to use Eclipse.

# Dec 7 8:51pm

Android doesn’t support two-way NFC, so users may have to touch twice to exchange their information.

But we may be able to do something clever with crypto instead. Since we started the conversation, the app can communicate details from the other user via the Internet instead of NFC.

I’m not sure if I’m using a shared secret or public/private keys yet. Keypairs will show off multi-threading better, so I’m likely going with that.

# Dec 7 9:07pm

I’m not sure what I’ll do for the concurrency requirement. I can’t reasonably implement the crypto on my own, and no libraries seem to use multiple threads for this. However, there’s likely a callback function that reports on the keypair generator’s progress. I could use this to inform the user of the keypair generation’s progress. This would probably require a multi-threaded message queue!

Never mind. This may not be compatible with Android’s threading and event model.

# Dec 7 9:21pm

I’ll need two main activities:
One will show a user list of people you’ve touched. It will include their name and a key fingerprint.

Clicking on a user will take you to the second activity: the chat screen. There is a textbox at the bottom allowing the user to send messages. The previous conversation will appear above.

I’ll also need an activity showing the progress of the initial key generation. I’ll probably throw in a bunch of technical details.

The NDK will be used as a library that exposes several important methods to the main Java application:

Keypair generation. The NDK returns a public key and a private key. The Java must remember these and use them in future calls. The public key can be shared over NFC. There will also be a mechanism for telling the Java how far we’ve gotten in generation.

Signing and encrypting a message. The NDK will take the user’s private key and an other’s public key to return a message that can only be read by the other.

Decrypting and verifying a message: The NDK will decrypt and verify a message that has been received in the chat.

# Dec 7 9:39pm

Made default Android project with fragments for the chat list and each individual chat.

# Dec 7 10:30pm

Built openssl for Android and added it to the Android Studio project

# Dec 7 11:20pm

Building my own C code is proving harder than I thought. I’ll have to look into exactly how this can work.

# Dec 7 11:31pm

Just found out about extended deadline. Unfortunately, I have a lot of assignments due Tuesday and Wednesday as well, so depending on how long those take, I might not actually get more time.

# Dec 9 11:35pm

Unfortunately, the synchronization requirement may be harder than I thought. I don’t really have any shared memory between threads (unless joining counts as synchronization). I could do something with decrypting mass amounts of messages and perhaps merging them. I might be able to use a broadcast to tell the calling thread when there’s new data to be processed into the UI.

# Dec 10 12:44am

I have an outline for the tasks I need to get done:

NDK basic functions: generate key, encrypt message, decrypt message
NDK synchronization: probably dealing with message arrays somehow
NFC set broadcast of public key
NFC listen and create chat
Poll for entered chats - display fingerprint
Poll for new chat messages
Display + decrypt incoming messages (in mass if needed on start?)
Send message (encrypted)
Video of functionality (fake second side with Firebase?)
Writeup

# Dec 10 3:02pm

Got native code execution with a hello world on Android Studio.

# Dec 10 5:08pm

Made native functions for all four things I need natively.

I’m considering massively reducing the scope of the project so that I can finish.

# Dec 10 6:12pm

To finish, I’m removing some features that will take a lot of time to code but don’t satisfy the project requirements. TouchChat is no longer an appropriate name, but changing it would be a hassle.

Now the app will just display a list of global chat messages along with which fingerprint signed them. No NFC will be used. Just keypair generation, signing messages, verifying messages, and polling.

NDK basic functions: generate key, sign message, verify message
NDK synchronization: verify large set of messages in one go
	Not sure where synchronization will come in.
Poll for new chat messages
Display and verify incoming messages (in mass if needed on start?)
Send message (signed)
Video of functionality
Writeup

Unfortunately, synchronization will be hard to include. My verification of a large set involves splitting up a large amount of work into multiple threads. I can just join them, which doesn’t involve synchronization.

I can use a mutex for reading/writing the array. It’s a bit pointless because writing individual elements is thread-safe, but it will work.

# Dec 10 7:25pm

Rewrote function signatures for simpler version.

# Dec 10 8:05pm

I’m creating the keypair generation, but Android Studio says there’s a build failure because it doesn’t recognize openssl functions (undefined reference) even though I’ve header included them and put a compiled .so in the correct folder.

# Dec 10 8:16pm

Got it working. Had to use an ldLibs hack to include my own parameters.

Java configuration is not fun.

# Dec 10 8:50pm

Trying to debug some crashes during the generation process. Without valgrind, gdb, or even printf, this may be difficult.

Fixed it. Hooray for undocumented openssl functions!

# Dec 10 9:15pm

The format I was using created null bytes. I’m going to switch to a better encoding that will hopefully work better with a string.

# Dec 10 9:40pm

PEM format worked well for the public key. Unfortunately, the private key encoding requires a bunch of passphrase crap.

# Dec 10 10:12pm

Saved public and private key on first generation in Android SharedPreferences

# Dec 10 10:50pm

Working on signing now.

# Dec 10 11:16pm

Getting an extremely unusual crash where one of my JNI parameters is in the wrong place. It’s like the parameters are out of order. But all my definitions looks correct.

Fixed. Apparently even static methods require a this pointer.

# Dec 10 11:58pm

Finished all the native code except for the synchronization requirement. I made a simple iterative solution. I may come back to this if I have time, but doing it this way lets me focus on the UI logic.

# Dec 11 12:56am

Got a message to show up on Firebase! This is really cool.

# Dec 11 1:26am

Strange crash on mass message processing, which is odd because I already tested this with a sample message.

Whoops, might be related to each array length being zero.

Nope, it’s still happening.

It was a typing issue. Pro-tip: don’t cast object arrays to string arrays.

# Dec 11 2:50am

Finished the listview. It actually works now!

# Dec 11 3:05am

I wasn’t planning on rooting my Nexus 4, but I guess it’s required!

Have to install a custom recovery to flash SuperSU.

# Dec 11 3:29am

Just did the video. Turns out there’s a nifty adb way to do it via the videorecord shell command.

I haven’t put synchronization into the project yet, but the app appearance won’t be affected by that.

# Dec 11 3:37am

My synchronization design will be to run the signing in two different threads. If either thread finds an invalid signature, both prematurely abort and return immediately.

This will require a shared variable so that the threads can check if they need to give up.

# Dec 11 4:08am

Getting some segfaults and/or infinite thread spawning. Hard to debug without gdb, valgrind, and print statements.

Okay, I’m sure now:

12-11 04:10:44.302    3478-3624/? A/libc﹕ Fatal signal 11 (SIGSEGV), code 1, fault addr 0x98 in tid 3624 (tesec.touchchat)
12-11 04:10:44.303    3478-3625/? A/libc﹕ Fatal signal 11 (SIGSEGV), code 1, fault addr 0x98 in tid 3625 (tesec.touchchat)

One of my accesses on the other thread is causing a segfault.

I’m currently trying to figure out which statement causes this by inserting an exit(1) between statements. If it exits normally, I know it reached there. If it crashes, I know that it must have been a statement before.

# Dec 11 4:16am

I was afraid of that. It crashes on a GetObjectArrayElement access.

http://www.ibm.com/developerworks/library/j-jni/ Looks like each thread needs its own env. I can do that! I just need to pass a VM object to the worker, and it can get its own env.

According to this, http://developer.android.com/training/articles/perf-jni.html threads cannot make JNI calls until they are “attached”.

Unfortunately, the JNI documentation doesn’t seem to be a fan of explaining in which namespace functions are.

# Dec 11 4:36am

After resolving that attachment issue, it looks like Java doesn’t like me trying to pass jobjectArrays between threads. I’ll have to create c-versions of everything.

I just hope jint and jboolean are not as restrictive.

# Dec 11 5:01am

The move to C arrays for all threads worked! All threads only operate on C stuff now, which is cleaner. And the app no longer crashes!

Just tested it with some invalid signatures and everything looks stable!

# Dec 11 5:06am

Committing this journal and doing my writeup now.
