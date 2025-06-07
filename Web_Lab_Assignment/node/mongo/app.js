const express = require('express');
const { MongoClient } = require('mongodb');
const path = require('path'); // Added for serving HTML file

const app = express();
const port = 3000;

const url = 'mongodb://127.0.0.1:27017';
const dbName = 'myProject';
const collectionName = 'student';

// Helper function to connect to MongoDB, perform an operation, and close connection
async function performDbOperation(operation) {
  const client = new MongoClient(url);
  try {
    await client.connect();
    const db = client.db(dbName);
    const collection = db.collection(collectionName);
    return await operation(collection);
  } finally {
    await client.close();
  }
}

// Function to insert dummy data (callable for reset)
async function insertDummyData(collection) {
    await collection.deleteMany({}); // Clear existing data
    const dummyStudents = [
      { USN: "101", name: "Alice", branch: "CSE", attendance: 25, grade: "A", companySelected: "Google" },
      { USN: "102", name: "Bob", branch: "ECE", attendance: 15, grade: "B", companySelected: null },
      { USN: "103", name: "Charlie", branch: "CSE", attendance: 30, grade: "A", companySelected: "Morgen Stanley" },
      { USN: "104", name: "David", branch: "MECH", attendance: 10, grade: "C", companySelected: null },
      { USN: "105", name: "Eve", branch: "CSE", attendance: 22, grade: "B", companySelected: null },
      { USN: "106", name: "Frank", branch: "ECE", attendance: 28, grade: "A", companySelected: "Intel" },
      { USN: "119", name: "Grace", branch: "CSE", attendance: 35, grade: "A", companySelected: "Morgen Stanley" },
      { USN: "120", name: "Heidi", branch: "CSE", attendance: 18, grade: "B", companySelected: null },
    ];
    await collection.insertMany(dummyStudents);
    return `Dummy data inserted into '${collectionName}' collection.`;
}

// Helper function to generate HTML table for students
function generateStudentListHtml(title, students, columns, emptyMessage) {
    let htmlResponse = `<h1>${title}</h1>`;
    if (students.length === 0) {
        htmlResponse += `<p>${emptyMessage}</p>`;
    } else {
        htmlResponse += '<table border="1"><tr>';
        columns.forEach(col => htmlResponse += `<th>${col.header}</th>`);
        htmlResponse += '</tr>';
        students.forEach(student => {
            htmlResponse += '<tr>';
            columns.forEach(col => {
                const value = student[col.key];
                htmlResponse += `<td>${value !== undefined && value !== null ? value : 'N/A'}</td>`;
            });
            htmlResponse += '</tr>';
        });
        htmlResponse += '</table>';
    }
    htmlResponse += '<br><a href="/">Go Back</a>';
    return htmlResponse;
}

// Serve home.html on the root route
app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, 'home.html'));
});

// Route to reset and re-insert dummy data
app.get('/resetdata', async (req, res) => {
  try {
    const message = await performDbOperation(async (collection) => {
        return await insertDummyData(collection);
    });
    res.send(`<p>${message}</p><br><a href="/">Go Back</a>`);
  } catch (e) {
    console.error('Error resetting data:', e);
    res.status(500).send(`Error resetting data: ${e.message} <br><a href="/">Go Back</a>`);
  }
});

// Route to view all students
app.get('/viewall', async (req, res) => {
  try {
    const students = await performDbOperation(async (collection) => {
        return await collection.find({}).toArray();
    });
    const columns = [
        { header: 'USN', key: 'USN' },
        { header: 'Name', key: 'name' },
        { header: 'Branch', key: 'branch' },
        { header: 'Attendance', key: 'attendance' },
        { header: 'Grade', key: 'grade' },
        { header: 'Company Selected', key: 'companySelected' }
    ];
    res.send(generateStudentListHtml('All Students', students, columns, 'No students found.'));
  } catch (e) {
    console.error('Error fetching all students:', e);
    res.status(500).send(`Error fetching students: ${e.message} <br><a href="/">Go Back</a>`);
  }
});

// 1: Print student whose attendance is less than 20.
app.get('/lessthan20', async (req, res) => {
  try {
    const students = await performDbOperation(async (collection) => {
        return await collection.find({ attendance: { $lt: 20 } }).toArray();
    });
    const columns = [
        { header: 'USN', key: 'USN' },
        { header: 'Name', key: 'name' },
        { header: 'Branch', key: 'branch' },
        { header: 'Attendance', key: 'attendance' }
    ];
    res.send(generateStudentListHtml('Students with Attendance < 20', students, columns, 'No students found with attendance less than 20.'));
  } catch (e) {
    console.error('Error fetching students with low attendance:', e);
    res.status(500).send(`Error: ${e.message} <br><a href="/">Go Back</a>`);
  }
});

// 2. Print all the student whose branch is CSE and who got selected for Morgen Stanley.
app.get('/csemorgan', async (req, res) => {
  try {
    const students = await performDbOperation(async (collection) => {
        return await collection.find({ branch: 'CSE', companySelected: 'Morgen Stanley' }).toArray();
    });
    const columns = [
        { header: 'USN', key: 'USN' },
        { header: 'Name', key: 'name' },
        { header: 'Branch', key: 'branch' },
        { header: 'Company Selected', key: 'companySelected' }
    ];
    res.send(generateStudentListHtml('CSE Students Selected for Morgen Stanley', students, columns, 'No CSE students found selected for Morgen Stanley.'));
  } catch (e) {
    console.error('Error fetching CSE Morgen Stanley students:', e);
    res.status(500).send(`Error: ${e.message} <br><a href="/">Go Back</a>`);
  }
});

// 3. Delete all the student whose branch is ECE.
app.get('/deleteece', async (req, res) => {
  try {
    const result = await performDbOperation(async (collection) => {
        return await collection.deleteMany({ branch: 'ECE' });
    });
    res.send(`<p>Deleted ${result.deletedCount} ECE students.</p><br><a href="/">Go Back</a>`);
  } catch (e) {
    console.error('Error deleting ECE students:', e);
    res.status(500).send(`Error: ${e.message} <br><a href="/">Go Back</a>`);
  }
});

// 4. Update student grade A to S where USN = "119"
app.get('/updategrade', async (req, res) => {
  try {
    const result = await performDbOperation(async (collection) => {
        return await collection.updateOne({ USN: '119', grade: 'A' }, { $set: { grade: 'S' } });
    });
    if (result.matchedCount === 0) {
        res.send('<p>No student found with USN "119" and grade "A" to update.</p><br><a href="/">Go Back</a>');
    } else {
        res.send(`<p>Updated ${result.modifiedCount} student's grade (USN '119' from A to S).</p><br><a href="/">Go Back</a>`);
    }
  } catch (e) {
    console.error('Error updating student grade:', e);
    res.status(500).send(`Error: ${e.message} <br><a href="/">Go Back</a>`);
  }
});

app.listen(port, () => {
  console.log(`App listening at http://localhost:${port}`);
  console.log(`Open http://localhost:${port} in your browser.`);
});