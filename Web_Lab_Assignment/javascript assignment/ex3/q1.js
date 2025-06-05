var score = 0;

function questionOne() { // Function Declaration
    var answer = prompt("Q1: What is 2 + 2 ? (a) 3 (b) 4 (c) 5");
    if (answer.toLowerCase() === "b" || answer.toLowerCase() === "4") {
        score++; // Update global score variable
        console.log("Correct!");
    } else {
        console.log("Wrong! The correct answer is 4.");
    }
}

const questionTwo = function () { // Function Expression
    let answer = prompt("Q2: What is 3 * 2? (a) 3 (b) 4 (c) 6");
    if (answer === "c" || answer === "6") {
        score++; // Update global score variable
        console.log("Correct!");
    } else {
        console.log("Wrong! The correct answer is 6.");
  }
};

const questionThree = function() { // Function Expression
    const answer = prompt("Q3: What color do you get when you mix red and white? (a) Pink (b) Orange (c) Purple");
    if (answer.toLowerCase() === "a" || answer.toLowerCase() === "pink") {
        score++; // Update global score variable
        console.log("Correct!");
    } else {
        console.log("Wrong! The correct answer is Pink.");
    }

    var localScore = score; // Local variable
};

console.log("hoistedVar before declaration:", hoistedVar); // used before declaration

var hoistedVar = "Hoisted Variable";

questionOne();
questionTwo();
questionThree();

console.log(`Your final score is: ${(score * 100) / 3}`);

function reviewScore() {
    try {
        console.log("Trying to access localScore from questionOne:", localScore);
    } catch (e) {
        console.log("Error accessing localScore:", e.message);
    }
}

reviewScore();