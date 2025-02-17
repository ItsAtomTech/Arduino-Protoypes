/* Passcode Validator
Written by Atomtech 2023*/
class SecurityKey {

private: char secretKey[10];
public: String keysEntered;

  //Passcode Setter, accepts string. MAX 8
  public:void setPassKey(String keys){
        keys.toCharArray(secretKey, keys.length()+1, 0);
        return;
  }  
  
  //Collate Input into String char buff
  public: void inputKeys(char ch){
    if(keysEntered.length() >= 8){
        return;
    }  
      
    String Consolidated = String(keysEntered) + String(ch);  
    keysEntered = Consolidated;
    Serial.println(keysEntered);
    return;
  }

  
  //Returns bool true if keys match the stored key;
  public: bool validateKeys(){
  if(keysEntered != String(secretKey)){
    return false;
  }
    return true;
  }

  //Clear current inputs 
  public: void clearInput(){
    keysEntered = "";    
    return;    
  }

    

};
