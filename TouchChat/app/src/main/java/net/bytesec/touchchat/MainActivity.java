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


public class MainActivity extends ActionBarActivity {

    SharedPreferences sharedPref;

    String pubkey;
    String privkey;

    EditText chatBox;
    ListView messageList;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

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
        String signature = Native.signMessage(message, privkey);

        System.out.println(message);
        System.out.println(signature);
        System.out.println(pubkey);
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
}
