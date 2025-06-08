const express = require('express');
const app = express();
const { MongoClient } = require('mongodb');
const path = require('path');

const uri = 'mongodb://127.0.0.1:27017';

app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, '8b.html'));
});

app.get('/insert', async (req, res) => {
  const { usn, name, company_name } = req.query;

  if (!usn || !name || !company_name) {
    return res.send('All fields are required');
  }

  let client;
  try {
    client = await MongoClient.connect(uri, { useUnifiedTopology: true });
    const db = client.db('placementdb');
    const collection = db.collection('finalyears');

    await collection.insertOne({ usn, name, company_name });

    res.send(`Student ${name} placement with ${company_name} registered successfully! <br><a href="/">Go Back</a>`);

  } catch (err) {
    console.error('Error:', err);
    res.status(500).send('Internal server error');
  } finally {
    if (client) {
      await client.close();
    }
  }
});

app.get('/infosys-students', async (req, res) => {
  let client;
  try {
    client = await MongoClient.connect(uri, { useUnifiedTopology: true });
    const db = client.db('placementdb');
    const collection = db.collection('finalyears');

    // Case-insensitive search for Infosys
    const infosysStudents = await collection.find({ 
      company_name: { $regex: /^infosys$/i } 
    }).toArray();

    let html = '<h2>Students Selected for Infosys</h2>';
    if (infosysStudents.length === 0) {
      html += '<p>No students found selected for Infosys.</p>';
    } else {
      html += '<table border="1" style="border-collapse: collapse;">';
      html += '<tr><th>USN</th><th>Name</th><th>Company</th></tr>';
      infosysStudents.forEach(student => {
        html += `<tr>
          <td>${student.usn}</td>
          <td>${student.name}</td>
          <td style="background-color: #ADD8E6; font-weight: bold;">${student.company_name}</td>
        </tr>`;
      });
      html += '</table>';
      html += `<p><strong>Total students selected for Infosys: ${infosysStudents.length}</strong></p>`;
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
    client = await MongoClient.connect(uri, { useUnifiedTopology: true });
    const db = client.db('placementdb');
    const collection = db.collection('finalyears');

    const allStudents = await collection.find({}).toArray();

    let html = '<h2>All Placed Students</h2>';
    if (allStudents.length === 0) {
      html += '<p>No students found.</p>';
    } else {
      html += '<table border="1" style="border-collapse: collapse;">';
      html += '<tr><th>USN</th><th>Name</th><th>Company</th></tr>';
      allStudents.forEach(student => {
        html += `<tr>
          <td>${student.usn}</td>
          <td>${student.name}</td>
          <td>${student.company_name}</td>
        </tr>`;
      });
      html += '</table>';
      html += `<p><strong>Total placed students: ${allStudents.length}</strong></p>`;
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

app.listen(5008, () => {
  console.log('Q8(b) - Listening on port 5008');
}); 