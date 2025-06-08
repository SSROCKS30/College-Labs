const express = require('express');
const app = express();
const { MongoClient } = require('mongodb');
const path = require('path');

const uri = 'mongodb://127.0.0.1:27017';

app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, '10b.html'));
});

app.get('/insert', async (req, res) => {
  const { id, title, name, branch } = req.query;

  let client;
  try {
    client = await MongoClient.connect(uri);
    const db = client.db('facultydb');
    const collection = db.collection('faculty');

    await collection.insertOne({ id, title, name, branch });

    res.send(`Faculty ${name} with title ${title} in ${branch} branch added successfully! <br><a href="/">Go Back</a>`);

  } catch (err) {
    console.error('Error:', err);
    res.status(500).send('Internal server error');
  } finally {
    if (client) {
      await client.close();
    }
  }
});

app.get('/cse-professors', async (req, res) => {
  let client;
  try {
    client = await MongoClient.connect(uri);
    const db = client.db('facultydb');
    const collection = db.collection('faculty');

    const cseProfessors = await collection.find({ 
      branch: 'CSE', 
      title: 'PROFESSOR' 
    }).toArray();

    let html = '<h2>CSE Department Professors</h2>';
    if (cseProfessors.length === 0) {
      html += '<p>No professors found in CSE department.</p>';
    } else {
      html += '<ul>';
      cseProfessors.forEach(faculty => {
        html += `<li> Faculty ID: ${faculty.id} | Name: ${faculty.name} | Title: ${faculty.title} | Branch: ${faculty.branch} </li>`;
      });
      html += '</ul>';
      html += `<p><strong>Total CSE Professors: ${cseProfessors.length}</strong></p>`;
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

app.get('/all-faculty', async (req, res) => {
  let client;
  try {
    client = await MongoClient.connect(uri);
    const db = client.db('facultydb');
    const collection = db.collection('faculty');

    const allFaculty = await collection.find({}).toArray();

    let html = '<h2>All Faculty Members</h2>';
    if (allFaculty.length === 0) {
      html += '<p>No faculty found.</p>';
    } else {
      html += '<ul>';
      allFaculty.forEach(faculty => {
        html += `<li> Faculty ID: ${faculty.id} | Name: ${faculty.name} | Title: ${faculty.title} | Branch: ${faculty.branch} </li>`;
      });
      html += '</ul>';
      html += `<p><strong>Total faculty: ${allFaculty.length}</strong></p>`;
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

app.listen(5010, () => {
  console.log('Q10(b) - Listening on port 5010');
}); 