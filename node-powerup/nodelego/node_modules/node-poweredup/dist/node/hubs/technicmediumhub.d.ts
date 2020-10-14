import { Peripheral } from "@abandonware/noble";
import { IBLEAbstraction } from "../interfaces";
import { LPF2Hub } from "./lpf2hub";
/**
 * The TechnicMediumHub is emitted if the discovered device is a Technic Medium Hub.
 * @class TechnicMediumHub
 * @extends LPF2Hub
 * @extends BaseHub
 */
export declare class TechnicMediumHub extends LPF2Hub {
    static IsTechnicMediumHub(peripheral: Peripheral): boolean;
    constructor(device: IBLEAbstraction);
    connect(): Promise<void>;
}
export declare const PortMap: {
    [portName: string]: number;
};
