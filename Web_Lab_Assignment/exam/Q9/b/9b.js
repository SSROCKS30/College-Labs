const express = require('express');
const app = express();
const { MongoClient } = require('mongodb');
const path = require('path');

const uri = 'mongodb://127.0.0.1:27017';

app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, '9b.html'));
});

app.get('/insert', async (req, res) => {
  const { name, branch, semester } = req.query;

  if (!name || !branch || !semester) {
    return res.send('All fields are required');
  }

  let client;
  try {
    client = await MongoClient.connect(uri);
    const db = client.db('studentbranchdb');
    const collection = db.collection('students');

    await collection.insertOne({ 
      name: name, 
      branch: branch, 
      semester: parseInt(semester) 
    });

    res.send(`Student ${name} from ${branch} branch, ${semester} semester added successfully! <br><a href="/">Go Back</a>`);

  } catch (err) {
    console.error('Error:', err);
    res.status(500).send('Internal server error');
  } finally {
    if (client) {
      await client.close();
    }
  }
});

app.get('/cse-6th-sem', async (req, res) => {
  let client;
  try {
    client = await MongoClient.connect(uri);
    const db = client.db('studentbranchdb');
    const collection = db.collection('students');

    const cse6thStudents = await collection.find({ 
      branch: 'CSE', 
      semester: 6 
    }).toArray();

    let html = '<h2>CSE 6th Semester Students</h2>';
    if (cse6thStudents.length === 0) {
      html += '<p>No CSE 6th semester students found.</p>';
    } else {
      html += '<ul>';
      cse6thStudents.forEach(student => {
        html += `<li> Name: ${student.name} | Branch: ${student.branch} | Semester: ${student.semester} </li>`;
      });
      html += '</ul>';
      html += `<p><strong>Total CSE 6th semester students: ${cse6thStudents.length}</strong></p>`;
    }
    html += '<br><a href="/">Go Back</a>';
    
    res.send(html);

  } catch (err) {
    console.error('Error:', err);
    res.status(500).send('Internal server error');
  } finally {
    if (client) {
      await client.close();
    }
  }
});

app.get('/all-students', async (req, res) => {
  let client;
  try {
    client = await MongoClient.connect(uri);
    const db = client.db('studentbranchdb');
    const collection = db.collection('students');

    const allStudents = await collection.find({}).toArray();

    let html = '<h2>All Students</h2>';
    if (allStudents.length === 0) {
      html += '<p>No students found.</p>';
    } else {
      html += '<ul>';
      allStudents.forEach(student => {
        html += `<li> Name: ${student.name} | Branch: ${student.branch} | Semester: ${student.semester} </li>`;
      });
      html += '</ul>';
      html += `<p><strong>Total students: ${allStudents.length}</strong></p>`;
    }
    html += '<br><a href="/">Go Back</a>';
    
    res.send(html);

  } catch (err) {
    console.error('Error:', err);
    res.status(500).send('Internal server error');
  } finally {
    if (client) {
      await client.close();
    }
  }
});

app.listen(5009, () => {
  console.log('Q9(b) - Listening on port 5009');
}); 