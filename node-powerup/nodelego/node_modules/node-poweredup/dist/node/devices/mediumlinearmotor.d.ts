import { TachoMotor } from "./tachomotor";
import { IDeviceInterface } from "../interfaces";
/**
 * @class MediumLinearMotor
 * @extends TachoMotor
 */
export declare class MediumLinearMotor extends TachoMotor {
    constructor(hub: IDeviceInterface, portId: number);
}
