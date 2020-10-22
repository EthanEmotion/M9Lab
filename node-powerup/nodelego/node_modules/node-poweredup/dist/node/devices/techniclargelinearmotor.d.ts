import { AbsoluteMotor } from "./absolutemotor";
import { IDeviceInterface } from "../interfaces";
/**
 * @class TechnicLargeLinearMotor
 * @extends AbsoluteMotor
 */
export declare class TechnicLargeLinearMotor extends AbsoluteMotor {
    constructor(hub: IDeviceInterface, portId: number);
}
