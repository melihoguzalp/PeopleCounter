package com.melihoguzalp.peoplecounter
import android.content.Context
import android.os.Bundle
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import com.ayware.thingspeakapi.OnTaskListener
import com.ayware.thingspeakapi.SortType
import com.ayware.thingspeakapi.ThingspeakGetDataTask
import com.ayware.thingspeakapi.models.Feed
import java.text.SimpleDateFormat
import java.util.*
import kotlin.collections.ArrayList

class MainActivity : AppCompatActivity() {


    var mTvEntered: TextView? = null
    var mTvOut: TextView? = null
    var mTvLastUpdated: TextView? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        mTvEntered = findViewById(R.id.tv_enteredCount)
        mTvOut = findViewById(R.id.tv_outCount)
        mTvLastUpdated = findViewById(R.id.tv_lastUpdatedTime)

        val dataTask = ThingspeakGetDataTask(this,object: OnTaskListener{

            override fun onDataGet(p0: MutableList<Feed>?, p1: java.lang.Exception?) {

                val size = p0?.size

                if (size != null) {

                    if(size > 0){

                        val feedLast = p0.get(size-1) //en son gelen jsondaki değeri alıyor.
                        val mapFeed = feedLast.mapField

                        val countEntered = mapFeed.get("People In")
                        val countOut = mapFeed.get("People Out")

                        mTvEntered?.setText(countEntered)
                        mTvOut?.setText(countOut)

                        val cal = Calendar.getInstance()
                        val sdf = SimpleDateFormat("dd-MM-yyyy HH:mm:ss",Locale.ENGLISH)
                        val lastDate: String = sdf.format(Date(cal.timeInMillis))

                        mTvLastUpdated?.setText("Last Updated:\n"+lastDate)

                    }
                }

            }

        })

        dataTask.setRepeat(1000)
            .setSort(SortType.ASC)
            .execute("https://api.thingspeak.com/channels/924094/feeds.json?api_key=8152CS4JYC09WD9I")

        // val intent = intent

        //    val NameList = ArrayList<String>()
        //  val SurnameList = ArrayList<String>()
        // val IdList = ArrayList<Int>()


        // val arrayAdapter = ArrayAdapter(this,android.R.layout.simple_list_item_1,NameList)
        //listView.adapter = arrayAdapter



        // try {


        //   val database = this.openOrCreateDatabase("People", Context.MODE_PRIVATE, null)


        // val cursor = database.rawQuery("SELECT * FROM people", null)
        //val peopleNameIx = cursor.getColumnIndex("name")
        //val peopleSurnameIx = cursor.getColumnIndex("surname")
        //val idIx = cursor.getColumnIndex("id")

        //  while (cursor.moveToNext()) {

        //    NameList.add(cursor.getString(peopleNameIx)+ " " +cursor.getString(peopleSurnameIx))
        //  IdList.add(cursor.getInt(idIx))


        // }


        // arrayAdapter.notifyDataSetChanged()

        // cursor.close()
        // }
        // catch (e : Exception){

        //   e.printStackTrace()
        // }

        val DateList = ArrayList<String>()
        val ConditionList = ArrayList<Int>()
        val IdList = ArrayList<Int>()

        // val arrayAdapter = ArrayAdapter(this,android.R.layout.simple_list_item_1,DateList)
        // listView.adapter = arrayAdapter

        try{

            val myDatabase = this.openOrCreateDatabase("People", Context.MODE_PRIVATE,null)
            myDatabase.execSQL("CREATE TABLE IF NOT EXISTS people (id INTEGER PRIMARY KEY,date VARCHAR, condition INT)")
            myDatabase.execSQL("INSERT INTO people (date,condition) VALUES('Melih',1)")
            myDatabase.execSQL("INSERT INTO people (date,condition) VALUES('Faruk',2)")

            //myDatabase.execSQL("INSERT INTO musicians (name,age) VALUES('Melih',23)")
            // myDatabase.execSQL("INSERT INTO musicians (name,age) VALUES('Serkan',14)")

            //  myDatabase.execSQL("UPDATE musicians SET age = 43 WHERE name ='Melih'")
            //   myDatabase.execSQL("UPDATE musicians SET name = 'Sebastian Larsson' WHERE id =3")
            // myDatabase.execSQL("DELETE FROM people WHERE name = 'Sebastian Larsson'")


            val cursor = myDatabase.rawQuery("SELECT * FROM people",null)


            val dateIx = cursor.getColumnIndex("date")
            val conditionIx = cursor.getColumnIndex("condititon")
            val idIx = cursor.getColumnIndex("id")



            while(cursor.moveToNext()){
                DateList.add(cursor.getString(dateIx))
                ConditionList.add(cursor.getInt(conditionIx))
                IdList.add(cursor.getInt(idIx))


            }


            // arrayAdapter.notifyDataSetChanged()

            cursor.close()

        }
        catch (e : Exception){

            e.printStackTrace()
        }




    }









}

