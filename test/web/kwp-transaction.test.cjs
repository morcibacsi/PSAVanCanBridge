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
    const page = loadPage().page;
    const definition = { protocol: 'kwp_hab', kwp_hab_telecoding_transaction: { enabled: true, transaction_zones: ['B0', 'B1', 'B2', 'B3', 'B8'], traceability_zone: 'A0', expected_payload_lengths_bytes: { B0: 2, B1: 2, B2: 1, B3: 1, B8: 1, A0: 2 } } };
    const payloads = { B0: 'AABB', B1: '1122', B2: '33', B3: '44', B8: '55', A0: '0001' };
    let requests = [];
    const transport = { send: async command => {
        requests.push(command);
        const zone = command.slice(2, 4);
        if (command.startsWith('21')) return '61' + zone + payloads[zone];
        if (command.startsWith('3B')) { payloads[zone] = command.slice(4); if (zone !== 'A0') payloads.A0 = '0002'; return '7B' + zone; }
        throw new Error(command);
    } };
    const client = new page.DiagnosticClient(transport, definition);
    const result = await client.writeKwpHabTelecodingTransaction({ B1: '6677' });
    assert.equal(result.verified.B1, '6677');
    assert.deepEqual(requests.filter(command => command.startsWith('3B')).map(command => command.slice(2, 4)), ['B0', 'B1', 'B2', 'B3', 'B8', 'A0']);
    assert.equal(requests.find(command => command.startsWith('3BA0')), '3BA00002');
    assert.equal(payloads.B0, 'AABB');
    requests = [];
    await client.writeKwpHabTelecodingTransaction({});
    assert.equal(requests.length, 0);
    await assert.rejects(client.writeKwpHabTelecodingTransaction({ B1: '00' }));
    assert.equal(requests.filter(command => command.startsWith('3B')).length, 0);
    requests = [];
    transport.send = async command => { requests.push(command); return command.startsWith('21') ? '61' + command.slice(2, 4) + payloads[command.slice(2, 4)] : '7BB9'; };
    await assert.rejects(client.writeKwpHabTelecodingTransaction({ B1: '6677' }));
    assert.equal(requests.filter(command => command.startsWith('3B')).length, 1);
    definition.kwp_hab_telecoding_transaction.transaction_zones = ['B1', 'B1'];
    assert.throws(() => client.getKwpHabTelecodingTransactionConfig());
    console.log('KWP-HAB transaction regression tests passed');
}
main().catch(error => { console.error(error); process.exitCode = 1; });
