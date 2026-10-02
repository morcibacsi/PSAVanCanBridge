const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');
const assert = require('node:assert/strict');
function loadPage() {
    const html = fs.readFileSync(path.join(__dirname, '../../data/telecoding.html'), 'utf8');
    const element = () => ({ value: '', innerHTML: '', style: {}, appendChild() {}, addEventListener() {}, querySelectorAll() { return []; }, replaceChildren() {}, classList: { add() {}, remove() {} } });
    const context = vm.createContext({ console, Uint8Array, setTimeout, clearTimeout, setInterval, clearInterval,
        document: { getElementById: element, createElement: element, addEventListener() {}, querySelector() { return null; } },
        window: { location: { host: 'test' } }, confirm: () => true });
    vm.runInContext(html.match(/<script>([\s\S]*)<\/script>/)[1] + '\nglobalThis.page = { DiagnosticClient, app, renderTabs, readAllZones };', context);
    return context;
}
async function main() {
    const context = loadPage();
    const page = context.page;
    for (const [protocol, zone, response, expected] of [['kwp_hab', 'b1', '61B1ABCD', 'ABCD'], ['uds', 'f190', '62F1905646', '5646']]) {
        const client = new page.DiagnosticClient({ send: async () => response }, { protocol });
        assert.equal(await client.readZone(zone), expected);
    }
    for (const response of ['61B2ABCD', '62B1ABCD', '61B1ABC', '7F2131']) {
        const client = new page.DiagnosticClient({ send: async () => response }, { protocol: 'kwp_hab' });
        await assert.rejects(client.readZone('B1'));
    }
    page.app.ecuDefinition = { tabs: { 1: 'Configuration' }, zones: { B1: { tab: 1, params: [] }, B2: { params: [] }, B3: { tab: 'extra', params: [] } } };
    vm.runInContext('createZoneCard = () => ({}); refreshControls = () => {}; updateZoneCard = () => {};', context);
    page.renderTabs();
    assert.deepEqual(Object.keys(page.app.zoneStates), ['B1', 'B2', 'B3']);
    let closed = 0;
    page.app.diagnosticClient = { selectECU: async () => {}, startSession: async () => {}, readZone: async zone => { if (zone === 'B2') throw new Error('Unsupported zone'); return 'AA'; }, stopSession: async () => { closed++; } };
    await page.readAllZones();
    assert.equal(page.app.zoneStates.B3.originalHex, 'AA');
    assert.equal(closed, 1);
    const kwpFault = vm.runInContext("parsePsaKwpCompactDtcResponse('5701F303')", context);
    assert.equal(kwpFault.faults[0].code, 'F303');
    const udsFault = vm.runInContext("parseUds1902DtcResponse('5902FF12345608')", context);
    assert.equal(udsFault.faults[0].code, '123456');
    assert.throws(() => vm.runInContext("parsePsaKwpCompactDtcResponse('5702F303')", context));
    console.log('Telecoding regression tests passed');
}
main().catch(error => { console.error(error); process.exitCode = 1; });
