package gr.fhw.lockcompanionapp;


import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;


public class MainActivity extends AppCompatActivity {

    private Button btnLock, btnUnlock, btnAuto, btnChangePassword;
    private EditText editTextPassword;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_main);

        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });

        // Initialize UI elements
        btnLock = findViewById(R.id.btn_lock);
        btnUnlock = findViewById(R.id.btn_unlock);
        btnAuto = findViewById(R.id.btn_auto);
        btnChangePassword = findViewById(R.id.btn_change_password);
        editTextPassword = findViewById(R.id.editTextNumberPassword);

        editTextPassword.setText(getPassword());

        // Set click listeners
        btnLock.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendHttpRequest("192.168.1.10", "post", "setLockState","2");
            }
        });

        btnUnlock.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendHttpRequest("192.168.1.10", "post", "setLockState","0");
            }
        });

        btnAuto.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendHttpRequest("192.168.1.10", "post", "setLockState","1");
            }
        });

        btnChangePassword.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String newPassword = editTextPassword.getText().toString().trim();
                if (!newPassword.isEmpty()) {

                    sendHttpRequest("192.168.1.10", "post", "setLockPassword", newPassword);
                    //Toast.makeText(MainActivity.this, "Κωδικός άλλαξε σε: " + newPassword, Toast.LENGTH_SHORT).show();
                    editTextPassword.setText(getPassword()); // Clear input after changing password
                } else {
                    Toast.makeText(MainActivity.this, "Παρακαλώ εισάγετε νέο κωδικό!", Toast.LENGTH_SHORT).show();
                }
            }
        });
    }

    private String getPassword() {
        String password;

        password = get("192.168.1.10", "post", "getLockPassword");

        return password;
    }

    private String getState() {
        String state;

        state = get("192.168.1.10", "post", "getLockState");

        return state;
    }

    private String get(String ip, String endpoint, String action) {
        new Thread(() -> {

            // Send HTTP request
            JSONObject response = NetworkUtils.makeHttpRequest(ip, endpoint, action, "");

            // Process response
            if (response != null) {
                if (action.equals("getLockState") && response.has("state")) {
                    String successMessage = response.optString("state", "error");
                    runOnUiThread(() ->
                            //Toast.makeText(MainActivity.this, "Success: " + successMessage, Toast.LENGTH_SHORT).show()
                    );
                } else if (action.equals("getLockPassword") && response.has("password")) {
                    String successMessage = response.optString("password", "error");
                    runOnUiThread(() ->{
                                editTextPassword.setText(successMessage);
                                //Toast.makeText(MainActivity.this, "Success: " + successMessage, Toast.LENGTH_SHORT).show();
                            }
                    );
                } else if (response.has("error")) {
                    String errorMessage = response.optString("error", "Unknown error occurred");
                    runOnUiThread(() ->
                            //Toast.makeText(MainActivity.this, "Error: " + errorMessage, Toast.LENGTH_SHORT).show()
                    );
                } else {
                    runOnUiThread(() ->
                            //Toast.makeText(MainActivity.this, "Unexpected response from server!", Toast.LENGTH_SHORT).show()
                    );
                }
            } else {
                runOnUiThread(() ->
                        Toast.makeText(MainActivity.this, "Failed to communicate with server!", Toast.LENGTH_SHORT).show()
                );
            }


        }).start();

        //
        return null;
    }
    private String sendHttpRequest(String ip, String endpoint, String action, String argument) {
        new Thread(() -> {

            // Send HTTP request
            JSONObject response = NetworkUtils.makeHttpRequest(ip, endpoint, action, argument);

            // Process response
            if (response != null) {
                if (response.has("success")) {
                    String successMessage = response.optString("success", "Action executed successfully");
                    runOnUiThread(() ->
                            Toast.makeText(MainActivity.this, "Success: " + successMessage, Toast.LENGTH_SHORT).show()
                    );
                } else if (response.has("error")) {
                    String errorMessage = response.optString("error", "Unknown error occurred");
                    runOnUiThread(() ->
                            Toast.makeText(MainActivity.this, "Error: " + errorMessage, Toast.LENGTH_SHORT).show()
                    );
                } else {
                    runOnUiThread(() ->
                            Toast.makeText(MainActivity.this, "Unexpected response from server!", Toast.LENGTH_SHORT).show()
                    );
                }
            } else {
                runOnUiThread(() ->
                        Toast.makeText(MainActivity.this, "Failed to communicate with server!", Toast.LENGTH_SHORT).show()
                );
            }

        }).start();

        return null;
    }




}