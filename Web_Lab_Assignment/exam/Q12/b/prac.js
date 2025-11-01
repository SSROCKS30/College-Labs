const express = require('express');
const app = express();
const {MongoClient} = require('mongodb');
const path = require('path');

const uri = 'mongodb://127.0.0.1:27017';

app.get('/', async (req, res) => {
    res.sendFile(__dirname + '/12b.html');
});

app.get('/insert', async (req, res) => {
    const {name, usn, subject, marks} = req.query;
    const parsedMarks = parseInt(marks);
    let client;

    try{
        client = await MongoClient.connect(uri);
        const db = client.db('exammarksdb');
        const collection = db.collection('student_marks');

        await collection.insertOne({name: name, usn: usn, subject: subject, marks: parsedMarks});
        let html = `<h2>Student ${name} has been added!</h2> <br> <br> <a href = "/">Go back</a>`;

        res.send(html);
    }
    catch(err){
        console.log(Error);
    }finally{
        if(client){
            await client.close();
        }
    }
});

app.get('/low-marks', async (req, res) => {
    let client;
    try{
        client = await MongoClient.connect(uri);
        const db = client.db('exammarksdb');
        const collection = db.collection('student_marks');

        const students = await collection.find({marks : { $lt: 20}}).toArray();
        let html = `<ul>`;
        students.forEach(student => {
            html += `<li> Name: ${student.name} | Marks: ${student.marks}</li>`
        });
        html += `</ul>`;

        res.send(html);
    }
    catch(err){
        console.log(Error);
    }finally{
        if(client){
            await client.close();
        }
    }
});

app.listen(3005, () => {
    console.log("server started");
});