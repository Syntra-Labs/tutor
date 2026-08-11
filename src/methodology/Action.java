package methodology;

import java.util.List;

//Action system
enum ActionType { OBSERVE, QUESTION, TEST, FALSIFY }

class Action {
 private final ActionType type;
 private final String description;
 private final List<Hypothesis> affectedHypotheses;
 private final Double likelihoodImpact;
 private final Boolean success;
 
 public Action(ActionType type, String description, List<Hypothesis> hypotheses, 
              Double likelihoodImpact, Boolean success) {
     this.type = type;
     this.description = description;
     this.affectedHypotheses = hypotheses;
     this.likelihoodImpact = likelihoodImpact;
     this.success = success;
 }
 
 // Getters
 public ActionType getType() { return type; }
 public String getDescription() { return description; }
 public List<Hypothesis> getAffectedHypotheses() { return affectedHypotheses; }
 public double getLikelihoodImpact() { return likelihoodImpact; }
 public Boolean getSuccess() { return success; }
}
