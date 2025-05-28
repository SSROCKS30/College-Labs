const express = require('express');
const { MongoClient } = require('mongodb');

const app = express();
const port = 3000;

const url = 'mongodb://127.0.0.1:27017';
const client = new MongoClient(url);

const dbName = 'myProject';

async function main() {
  try {
    await client.connect();
    console.log('Connected successfully to MongoDB server');
    const db = client.db(dbName);
    const collection = db.collection('student');

    // Clear existing data in the student collection
    await collection.deleteMany({});
    console.log('Cleared existing data from "student" collection.');

    // Add dummy data
    const dummyStudents = [
      { USN: "101", name: "Alice", branch: "CSE", attendance: 25, grade: "A", companySelected: "Google" },
      { USN: "102", name: "Bob", branch: "ECE", attendance: 15, grade: "B", companySelected: null }, // ECE, attendance < 20
      { USN: "103", name: "Charlie", branch: "CSE", attendance: 30, grade: "A", companySelected: "Morgen Stanley" }, // CSE, Morgen Stanley
      { USN: "104", name: "David", branch: "MECH", attendance: 10, grade: "C", companySelected: null }, // attendance < 20
      { USN: "105", name: "Eve", branch: "CSE", attendance: 22, grade: "B", companySelected: null },
      { USN: "106", name: "Frank", branch: "ECE", attendance: 28, grade: "A", companySelected: "Intel" }, // ECE
      { USN: "119", name: "Grace", branch: "CSE", attendance: 35, grade: "A", companySelected: "Morgen Stanley" }, // USN 119, Grade A
      { USN: "120", name: "Heidi", branch: "CSE", attendance: 18, grade: "B", companySelected: null }, // CSE, attendance < 20
    ];
    await collection.insertMany(dummyStudents);
    console.log('Inserted dummy student data.');

    // 1: Print student whose attendance is less than 20.
    const lowAttendanceStudents = await collection.find({ attendance: { $lt: 20 } }).toArray();
    console.log('Query 1: Students with attendance < 20:', lowAttendanceStudents);

    // 2. Print all the student whose branch is CSE and who got selected for Morgen Stanley.
    const cseMorganStanleyStudents = await collection.find({ branch: 'CSE', companySelected: 'Morgen Stanley' }).toArray();
    console.log('Query 2: CSE students selected for Morgen Stanley:', cseMorganStanleyStudents);

    // 3. Delete all the student whose branch is ECE.
    const deleteEceResult = await collection.deleteMany({ branch: 'ECE' });
    console.log(`Query 3: Deleted ${deleteEceResult.deletedCount} ECE students.`);

    // 4. Update student grade A to S where USN = "119"
    const updateGradeResult = await collection.updateOne({ USN: '119', grade: 'A' }, { $set: { grade: 'S' } });
    console.log(`Query 4: Updated ${updateGradeResult.modifiedCount} student's grade (USN '119' from A to S).`);
    
    // Optional: Log all students after operations for verification
    const allStudentsAfterOps = await collection.find({}).toArray();
    console.log('All students in collection after operations:', allStudentsAfterOps);

    return 'Database operations complete. Check console for details.';
  } finally {
    // Ensures that the client will close when you finish/error
    await client.close();
    console.log('MongoDB connection closed.');
  }
}

app.get('/', async (req, res) => {
  try {
    const message = await main();
    res.send(message);
  } catch (e) {
    console.error('Error during database operations:', e);
    res.status(500).send(`Error performing database operations: ${e.message}`);
  }
});

app.listen(port, () => {
  console.log(`App listening at http://localhost:${port}`);
}); 