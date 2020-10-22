import { TachoMotor } from "./tachomotor";
import { IDeviceInterface } from "../interfaces";
/**
 * @class MoveHubMediumLinearMotor
 * @extends TachoMotor
 */
export declare class MoveHubMediumLinearMotor extends TachoMotor {
    constructor(hub: IDeviceInterface, portId: number);
}
