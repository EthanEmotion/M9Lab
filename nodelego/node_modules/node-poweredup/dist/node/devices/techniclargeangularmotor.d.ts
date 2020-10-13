import { AbsoluteMotor } from "./absolutemotor";
import { IDeviceInterface } from "../interfaces";
import * as Consts from "../consts";
/**
 * @class TechnicLargeAngularMotor
 * @extends AbsoluteMotor
 */
export declare class TechnicLargeAngularMotor extends AbsoluteMotor {
    constructor(hub: IDeviceInterface, portId: number, modeMap?: {
        [event: string]: number;
    }, type?: Consts.DeviceType);
}
