// function for start and stop adding
void zapusk() {
  if (enc1.isHolded() ){
    zapuskData[currentServo] = !zapuskData[currentServo];
    //Serial.println(zapuskData[currentServo]); // debugging
    printScreen();

  }
}

// computation of final position of Servo
void changeEndPositions (byte i) {
  endPositions[i] = parameters[1][i] - parameters[4][i] * volCoefs[i]; // final = Positions - Volume * Volcoef
}

void adding() {
  for (byte i = 0; i < AMOUNT; i++) {
    if (zapuskData[i] == 1) {
      if (parameters[1][i] > endPositions[i] && parameters[5][i] == 0) {
        addMode0 (i);
      }
      else if (parameters[1][i] > endPositions[i] && parameters[5][i] == 1) {
        addMode1 (i);

        //Serial.println(endPositions[i]); // debug
      }
    }
  }
}

// linear Mode, constant rate
void addMode0 (byte i) {
  float dt = (millis() - last_time[i]) / 1000.00001; // ti
  if ( dt >= times[i] * increment / (parameters[4][i] * volCoefs[i] )) {
    parameters[1][i] -= increment;

    //Serial.println(dt);
    //Serial.println(times[i]);
    //Serial.println(last_time[i]);

    last_time[i] = millis();

    //Serial.println(_time / (parameters[4][i] * volCoef ));
    //Serial.println(millis());

    printScreen();
    Serial.println(parameters[1][i]);
    syr[i].writeMicroseconds(parameters[1][i]);
  }
}


//exponential Mode
void addMode1 (byte i) {
  float k = log(parameters[4][i] / 20) / times[i]; // calculation of exponent constant
  float dt = (millis() - last_time[i]) / 1000.00001;
  if (dt >= (increment / volCoefs[i]) / (k * exp(-k * reaction_times[i])*parameters[4][i])) {
    parameters[1][i] -=  increment;
    reaction_times[i] += dt;


    //Serial.println(k,7);
    //Serial.println((millis() - last_time[i]) / 1000);
    //Serial.println(last_time[i]);
    //Serial.println(reaction_times[i]);


    last_time[i] = millis();


    //Serial.println(millis());
    //Serial.println(parameters[1][i]);

    printScreen();
    syr[i].writeMicroseconds(parameters[1][i]);
  }

}
