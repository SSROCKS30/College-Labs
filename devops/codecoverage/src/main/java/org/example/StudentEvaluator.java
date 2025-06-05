package org.example;

import java.util.ArrayList;
import java.util.List;

public class StudentEvaluator {
    static class Student {
        String name;
        int marks;

        public Student(String name, int marks) {
            if (name == null || name.trim().isEmpty()) {
                throw new IllegalArgumentException("Name cannot be empty");
            }
            if (marks < 0 || marks > 100) {
                throw new IllegalArgumentException("Marks must be between 0 and 100");
            }
            this.name = name;
            this.marks = marks;
        }
    }

    private final List<Student> students = new ArrayList<>();

    public void addStudent(String name, int marks) {
        students.add(new Student(name, marks));
    }

    public double calculateAverage() {
        if (students.isEmpty()) return 0.0;
        int total = 0;
        for (Student s : students) {
            total += s.marks;
        }
        return (double) total / students.size();
    }

    public String evaluateGrade(int marks) {
        if (marks >= 90) return "A";
        else if (marks >= 75) return "B";
        else if (marks >= 50) return "C";
        else return "Fail";
    }

    public int getStudentCount() {
        return students.size();
    }
}
