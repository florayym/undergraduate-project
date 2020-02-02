package com.example.gmy.explorehometown;

import android.content.Intent;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Toast;

public class ShotActivity extends AppCompatActivity {

    @Override//重写的创建菜单的方法
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.share_and_detail, menu);
        return true;//true表示显示出来
    }

    @Override//重写的点击菜单中选项的方法
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()){
            case R.id.share:
                Toast.makeText(ShotActivity.this, "请稍后，分享功能尚未上线", Toast.LENGTH_SHORT).show();
                break;
            case R.id.detail:
                Intent intent = new Intent(ShotActivity.this, DetailActivity.class);
                startActivity(intent);
                break;
            default:
                break;
        }
        return true;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_shot);

        //设置ActionBar颜色
        ActionBar ab = getSupportActionBar();



    }
}
