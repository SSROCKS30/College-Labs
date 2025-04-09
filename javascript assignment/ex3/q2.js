var studentName = "Sahil Sahay";
var activityScores = [25, 18, 12, 15];

function calculateTotal() {
  var total = 0;
  for (var i = 0; i < activityScores.length; i++) {
    total += activityScores[i];
  }
  return total;
}

var evaluateRank = function (totalScore) {
  if (totalScore > 40) return "Star Performer";
  else if (totalScore >= 30) return "Active";
  else return "Inactive";
};

var calculateAverage = function () {
  return calculateTotal() / activityScores.length;
};

function generateReport() {
  var total = calculateTotal();
  var average = calculateAverage();
  function showDetails() {
    console.log("Student:", studentName);
    console.log("Total Score:", total);
    console.log("Average Score:", average.toFixed(2));
    console.log("Rank:", evaluateRank(total));
  }
  showDetails();
}

console.log("Before declaration:", category);
var category = "General";

if (true) {
  var status = "Completed";
}
console.log("Accessed outside block:", status);

generateReport();
