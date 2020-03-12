package com.melihoguzalp.peoplecounter
import android.content.ContentValues
import android.content.Context
import android.database.sqlite.SQLiteDatabase
import android.database.sqlite.SQLiteOpenHelper
import android.provider.BaseColumns





object PeopleReader {
    // Table contents are grouped together in an anonymous object.
    object Entry : BaseColumns {
        const val TABLE_NAME = "Counter"
        const val COLUMN_NAME_TITLE = "People"
        const val COLUMN_NAME_SUBTITLE = "Date"
    }
}



private const val SQL_CREATE_ENTRIES =
    "CREATE TABLE ${PeopleReader.Entry.TABLE_NAME} (" +
            "${BaseColumns._ID} INTEGER PRIMARY KEY," +
            "${PeopleReader.Entry.COLUMN_NAME_TITLE} TEXT," +
            "${PeopleReader.Entry.COLUMN_NAME_SUBTITLE} TEXT)"


private const val SQL_DELETE_ENTRIES = "DROP TABLE IF EXISTS ${PeopleReader.Entry.TABLE_NAME}"



class PeopleReaderDbHelper(context: Context) : SQLiteOpenHelper(context, DATABASE_NAME, null, DATABASE_VERSION) {
    override fun onCreate(db: SQLiteDatabase) {
        db.execSQL(SQL_CREATE_ENTRIES)
    }
    override fun onUpgrade(db: SQLiteDatabase, oldVersion: Int, newVersion: Int) {
        // This database is only a cache for online data, so its upgrade policy is
        // to simply to discard the data and start over
        db.execSQL(SQL_DELETE_ENTRIES)
        onCreate(db)
    }
    override fun onDowngrade(db: SQLiteDatabase, oldVersion: Int, newVersion: Int) {
        onUpgrade(db, oldVersion, newVersion)
    }
    companion object {
        // If you change the database schema, you must increment the database version.
        const val DATABASE_VERSION = 1
        const val DATABASE_NAME = "PeopleReader.db"
    }

    var dbHelper = PeopleReaderDbHelper(context)


    // Gets the data repository in write mode
    var dbWriter: SQLiteDatabase? = dbHelper.writableDatabase

    // Create a new map of values, where column names are the keys
    val values = ContentValues().apply {


        put(PeopleReader.Entry.COLUMN_NAME_TITLE)
        put(PeopleReader.Entry.COLUMN_NAME_SUBTITLE)

    }




    // Insert the new row, returning the primary key value of the new row
    var newRowId = dbWriter?.insert(PeopleReader.Entry.TABLE_NAME, null, values)


     var dbReader = dbHelper.readableDatabase




    // Define a projection that specifies which columns from the database
// you will actually use after this query.
    val projection = arrayOf(BaseColumns._ID, PeopleReader.Entry.COLUMN_NAME_TITLE, PeopleReader.Entry.COLUMN_NAME_SUBTITLE)

    // Filter results WHERE "title" = 'My Title'
    val selection = "${PeopleReader.Entry.COLUMN_NAME_TITLE} = ?"
    val selectionArgs = arrayOf("My Title")

    // How you want the results sorted in the resulting Cursor
    val sortOrder = "${PeopleReader.Entry.COLUMN_NAME_SUBTITLE} DESC"

    val cursor = dbReader.query(
        PeopleReader.Entry.TABLE_NAME,   // The table to query
        projection,             // The array of columns to return (pass null to get all)
        selection,              // The columns for the WHERE clause
        selectionArgs,          // The values for the WHERE clause
        null,                   // don't group the rows
        null,                   // don't filter by row groups
        sortOrder               // The sort order
    )


    fun onDestroy() {
        dbHelper.close()
    }




}



private fun ContentValues.put(columnNameTitle: String) {
    TODO("not implemented") //To change body of created functions use File | Settings | File Templates.
}


