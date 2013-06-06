package teaonly.simplertc;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

public class MainActivity extends Activity {
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
    
        Button btnTest = (Button)findViewById(R.id.btn_test);
        btnTest.setOnClickListener(testAction);

        System.loadLibrary("simplertc_so");
    }


    private OnClickListener testAction = new OnClickListener() {
        @Override
        public void onClick(View v) {
            startRtcTask();
        }
    };
    
    static private native int startRtcTask();
}
