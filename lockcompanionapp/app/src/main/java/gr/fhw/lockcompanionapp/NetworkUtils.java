package gr.fhw.lockcompanionapp;

import android.util.Log;

import org.json.JSONObject;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;

public class NetworkUtils {

    public static JSONObject makeHttpRequest(String ipAddress, String endpoint, String action, String argument) {

        if (argument == null) {
            argument = ""; // Default value
        }

        // esp ip with http and endpoint (/post)
        String urlString = "http://" + ipAddress + "/" + endpoint;
        HttpURLConnection urlConnection = null;
        BufferedReader reader = null;
        StringBuilder responseStringBuilder = new StringBuilder();

        try {
            // Create URL object
            URL url = new URL(urlString);
            urlConnection = (HttpURLConnection) url.openConnection();
            urlConnection.setRequestMethod("POST");
            urlConnection.setConnectTimeout(6000);
            urlConnection.setReadTimeout(6000);
            urlConnection.setRequestProperty("Content-Type", "application/json");
            urlConnection.setRequestProperty("Accept", "application/json");
            urlConnection.setDoOutput(true);

            // Create JSON object with action and argument
            JSONObject jsonParam = new JSONObject();
            jsonParam.put("action", action);
            jsonParam.put("argument", argument);

            // Send JSON data
            DataOutputStream outputStream = new DataOutputStream(urlConnection.getOutputStream());
            outputStream.writeBytes(jsonParam.toString());
            outputStream.flush();
            outputStream.close();

            // Get response
            int responseCode = urlConnection.getResponseCode();
            if (responseCode == HttpURLConnection.HTTP_OK) {
                reader = new BufferedReader(new InputStreamReader(urlConnection.getInputStream()));
                String line;
                while ((line = reader.readLine()) != null) {
                    responseStringBuilder.append(line);
                }
                return new JSONObject(responseStringBuilder.toString()); // Convert response to JSON
            } else {
                Log.e("HTTP_ERROR", "Server returned response code: " + responseCode);
            }
        } catch (Exception e) {
            Log.e("HTTP_EXCEPTION", "Error: " + e.getMessage());
        } finally {
            if (urlConnection != null) urlConnection.disconnect();
            if (reader != null) {
                try {
                    reader.close();
                } catch (Exception e) {
                    Log.e("BUFFERED_READER", "Error closing reader: " + e.getMessage());
                }
            }
        }
        return null; // Return null if an error occurs
    }

}
