package protocols;

import java.util.ArrayList;
import java.util.List;

//Action designer for suggesting next steps
class ActionsDesigner {
 public List<String> designActions(EvaluationResult evalResult) {
     List<String> actions = new ArrayList<>();
     
     // Design actions based on evaluation result
     if (evalResult.score > 0.7) {
         actions.add("Continue current path");
     } else if (evalResult.score > 0.4) {
         actions.add("Adjust direction slightly");
         actions.add("Increase monitoring frequency");
     } else {
         actions.add("Change direction significantly");
         actions.add("Increase power output");
         actions.add("Request assistance");
     }
     
     return actions;
 }
}