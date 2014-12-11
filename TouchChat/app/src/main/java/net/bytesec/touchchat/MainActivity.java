package net.bytesec.touchchat;

import android.content.Context;
import android.content.SharedPreferences;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.TextView;

import com.firebase.client.ChildEventListener;
import com.firebase.client.DataSnapshot;
import com.firebase.client.Firebase;
import com.firebase.client.FirebaseError;
import com.firebase.client.Query;
import com.firebase.client.ValueEventListener;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;


public class MainActivity extends ActionBarActivity {

    SharedPreferences sharedPref;

    String pubkey;
    String privkey;

    EditText chatBox;
    ListView messageList;

    Firebase firebase;

    List<String> seenKeys;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Firebase.setAndroidContext(this);
        firebase = new Firebase("https://flickering-inferno-775.firebaseio.com/");
        seenKeys = new ArrayList<>();

        System.loadLibrary("nativecrypt");

        chatBox = (EditText)findViewById(R.id.chatBox);
        messageList = (ListView)findViewById(R.id.messageList);

        chatBox.setOnEditorActionListener(new ChatBoxListener());

        sharedPref = this.getPreferences(Context.MODE_PRIVATE);

        if (!sharedPref.contains("pubkey")) {
            String[] keypair = Native.generateKeypair().split("\\|");
            // Separate
            pubkey = keypair[0];
            privkey = keypair[1];

            SharedPreferences.Editor editor = sharedPref.edit();
            editor.putString("pubkey", pubkey);
            editor.putString("privkey", privkey);
            editor.commit();
        } else {
            pubkey = sharedPref.getString("pubkey", "");
            privkey = sharedPref.getString("privkey", "");
        }

        // Do mass verification first to demonstrate C's parallelism
        processMassMessage();
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    public void sendMessage(String message) {
        if (message.length() == 0) {
            return;
        }

        String signature = Native.signMessage(message, privkey);

        Map<String, String> fullmsg = new HashMap<String, String>();
        fullmsg.put("message", message);
        fullmsg.put("signature", signature);
        fullmsg.put("pubkey", pubkey);

        firebase.child("messages").push().setValue(fullmsg);
    }

    // Call AFTER verifying signature
    public void processMessage(String message, String pubkey) {
        System.out.println(message);

    }

    public void processMassMessage() {
        Query q = firebase.child("messages").orderByKey();

        // Process all existing children en masse
        final List<String> messages = new ArrayList<>();
        final List<String> signatures = new ArrayList<>();
        final List<String> pubkeys = new ArrayList<>();

        // This will fire once with the entire contents of "messages".
        q.addListenerForSingleValueEvent(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot snapshot) {
                Map<String, Map<String, String>> allmsgs = (Map<String, Map<String, String>>) snapshot.getValue();

                for (Map.Entry<String, Map<String, String>> e : allmsgs.entrySet()) {
                    // Add all messages.
                    // They'll be out of order, but this is just a proof of concept
                    // and easily fixed in polishing.
                    messages.add(e.getValue().get("message"));
                    signatures.add(e.getValue().get("signature"));
                    pubkeys.add(e.getValue().get("pubkey"));
                    // Don't want to double-process
                    seenKeys.add(e.getKey());
                }

                String[] fortype = {};
                boolean all_verified = Native.verifyMassMessages(
                messages.toArray(fortype), signatures.toArray(fortype), pubkeys.toArray(fortype));

                if (all_verified) {
                    for(int i = 0; i < messages.size(); i++) {
                        // Add each message to the UI.
                        processMessage(messages.get(i), pubkeys.get(i));
                    }
                } else {
                    // This should never happen if someone didn't screw up the datastore.
                    System.out.println("We're under attack!!!");
                }

                // Ready for passive child listening
                firebase.child("messages").addChildEventListener(new MessageListener());
            }

            @Override
            public void onCancelled(FirebaseError firebaseError) {
            }
        });
    }

    public class ChatBoxListener implements TextView.OnEditorActionListener {
        @Override
        public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
            String message = chatBox.getText().toString();
            chatBox.setText("");

            sendMessage(message);

            return true;
        }
    }

    public class MessageListener implements ChildEventListener {

        @Override
        public void onChildAdded(DataSnapshot dataSnapshot, String s) {
            if (seenKeys.contains(dataSnapshot.getKey())) {
                // Don't process it again.
                return;
            }
            Map<String, String> fullmsg = (Map<String, String>) dataSnapshot.getValue();

            String message = fullmsg.get("message");
            String signature = fullmsg.get("signature");
            String pubkey = fullmsg.get("pubkey");
            if (Native.verifyMessage(message, signature, pubkey)) {
                processMessage(fullmsg.get("message"), fullmsg.get("pubkey"));
            }

            // Mark that we've seen this
            seenKeys.add(dataSnapshot.getKey());
        }

        @Override
        public void onChildChanged(DataSnapshot dataSnapshot, String s) {

        }

        @Override
        public void onChildRemoved(DataSnapshot dataSnapshot) {

        }

        @Override
        public void onChildMoved(DataSnapshot dataSnapshot, String s) {

        }

        @Override
        public void onCancelled(FirebaseError firebaseError) {

        }
    }
}
