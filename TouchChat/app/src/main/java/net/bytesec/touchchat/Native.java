package net.bytesec.touchchat;


public class Native {
    public static native void helloworld();

    // Returns a public key, pipe character, private key.
    public static native String generateKeypair();

    // Signs a message. Returns signature
    public static native String signMessage(String message, String yourPrivateKey);

    // Verifies a message from other
    public static native boolean verifyMessage(String message, String signature, String otherPublicKey);

    // Verifies a mass number of messages in parallel
    // Returns whether all messages are valid. (If false, then you are under attack.)
    public static native boolean verifyMassMessages(String[] messages, String[] otherPublicKeys);
}
