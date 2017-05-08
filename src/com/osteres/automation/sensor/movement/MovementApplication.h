//
// Created by Thibault PLET on 08/05/2017.
//

#ifndef COM_OSTERES_AUTOMATION_SENSOR_MOVEMENT_MOVEMENTAPPLICATION_H
#define COM_OSTERES_AUTOMATION_SENSOR_MOVEMENT_MOVEMENTAPPLICATION_H

/* Defined values */
#define IGNORE_PACKET_SUCCESS_RESPONSE true


#include <Arduino.h>
#include <com/osteres/automation/arduino/ArduinoApplication.h>
#include <com/osteres/automation/sensor/Identity.h>
#include <com/osteres/automation/sensor/movement/action/ActionManager.h>
#include <com/osteres/automation/sensor/movement/action/TransmitMovement.h>

using com::osteres::automation::arduino::ArduinoApplication;
using com::osteres::automation::sensor::Identity;
using com::osteres::automation::sensor::movement::action::ActionManager;
using com::osteres::automation::sensor::movement::action::TransmitMovement;

namespace com
{
    namespace osteres
    {
        namespace automation
        {
            namespace sensor
            {
                namespace movement
                {
                    class MovementApplication : public ArduinoApplication
                    {
                    public:
                        /**
                         * Sensor identifier
                         */
                        static byte const SENSOR = Identity::WEATHER;

                        /**
                         * Constructor
                         */
                        MovementApplication(
                            Transmitter *transmitter
                        ) : ArduinoApplication(MovementApplication::SENSOR, transmitter)
                        {
                            this->construct();
                        }

                        /**
                         * Destructor
                         */
                        virtual ~MovementApplication()
                        {
                            // Remove movement action
                            if (this->actionMovement != NULL) {
                                delete this->actionMovement;
                                this->actionMovement = NULL;
                            }
                        }

                        /**
                         * Setup application
                         */
                        virtual void setup()
                        {
                            // Parent
                            ArduinoApplication::setup();

                            // Transmission
                            this->transmitter->setActionManager(this->getActionManager());
                        }

                        /**
                         * Process application
                         */
                        virtual void process()
                        {
                            // Request an identifier if needed. Note: Not mandatory anymore
                            if (false && this->isNeedIdentifier()) {
                                this->requestForAnIdentifier();

                                // Send and listen
                                this->transmitter->srs(3000); // 3s

                            } // Process
                            else {

                                // Send movement signal
                                this->requestForSendData();

                                // Send battery level
                                this->requestForBatteryLevel();

                                // Send and listen
                                this->transmitter->srs();
                            }

                            // Wait 100ms
                            delay(100);
                        }

                        /**
                         * Send data to server
                         */
                        void requestForSendData()
                        {
                            // Process
                            this->getActionMovement()->execute();
                        }

                        /**
                         * Get action movement
                         */
                        TransmitMovement *getActionMovement()
                        {
                            if (this->actionMovement == NULL) {
                                this->actionMovement = new TransmitMovement(
                                    this->getPropertyType(),
                                    this->getPropertyIdentifier(),
                                    Identity::MASTER,
                                    this->transmitter
                                );
                            }

                            return this->actionMovement;
                        }

                    protected:

                        /**
                         * Common part constructor
                         */
                        void construct()
                        {
                            // Create action manager
                            ActionManager *actionManager = new ActionManager();
                            this->setActionManager(actionManager);
                        }

                        /**
                         * Action to transmit movement
                         */
                        TransmitMovement *actionMovement = NULL;
                    };
                }
            }
        }
    }
}


#endif //COM_OSTERES_AUTOMATION_SENSOR_MOVEMENT_MOVEMENTAPPLICATION_H
