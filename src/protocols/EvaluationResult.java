package protocols;

//Value object for evaluation results
class EvaluationResult {
 double score;
 String recommendation;
 
 EvaluationResult(double score, String recommendation) {
     this.score = score;
     this.recommendation = recommendation;
 }
}