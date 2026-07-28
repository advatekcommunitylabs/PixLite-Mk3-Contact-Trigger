# Security policy

This is a single-purpose controller intended for use on a trusted local
network, so significant security issues are considered unlikely in normal
installations. The project still errs on the side of caution. Responsible
reports are reviewed, sensitive details are handled privately, and users
should follow the deployment guidance below.

## Supported versions

Until a stable release exists, security fixes target the current beta branch
and the newest published beta release. Older generated sketches may not receive
backported fixes.

## Reporting a vulnerability

Do not open a public issue for a suspected vulnerability or include working
exploit details in a discussion.

Use GitHub's private **Report a vulnerability** flow for the repository. If
private vulnerability reporting is unavailable, contact the Advatek Labs
repository maintainers privately and ask for a secure reporting channel.

Include the affected version, attack preconditions, impact, reproduction steps,
and any proposed mitigation. Remove real credentials and customer network
details.

## Deployment model

The device provides local HTTP without TLS. Its optional password reduces
accidental local access. Install it on a trusted LAN or isolated control VLAN and restrict
access at the network boundary.

Configuration exports omit known password and hash fields. Diagnostics must
also remain credential-free. A change that exposes secrets, weakens recovery
boundaries, or allows unauthenticated mutation while protection is enabled is
security-sensitive.

The device is not designed for direct internet exposure.
