/* Tonix Tone Player
By Atomtech */

class Tonix {
	public: int buzz;
	private: unsigned long prevTime;
	public: int currentIndex = -1;
	

	
	
	//set the buzzer pin for the Tonix class
	public: void init(int pin){
		buzz = pin;
		
		return;
	}
	
	public: play(int index){
		currentIndex = index;
		
		return;
	}
	

	
	int counter = 0;

	//Activate Player Listening for event
	public: playOn(){	
					
		int tones[][4] = {
				{1000,1400,1800,0},
				{250,80,250,80},
				{100,10,0,0}
				
			};
					
		int size = 4;	
		if(currentIndex < 0){
			return;
		}
	
		if(millis() - prevTime >= 100){
			prevTime = millis();
				
		
			tone(buzz, tones[currentIndex][counter-1], 99);
			
		
			if(counter >= size){
				currentIndex = -1;
				counter = 0;
				digitalWrite(buzz, LOW);
				
			}else{
				counter++;
			}	

			
			
			
			
		}

	return;
	
}
	


	
	
	
	
	
};