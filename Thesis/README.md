
## Memo

### Environment
- Create with a blank hard disk
- VMware Workstation 15.x in Windows 10
- 1 processor with 2 cores per processor
- Memory size 4096 MB
- Maximum disk size 50.0 GB
- ubuntu-18.04.3-desktop-amd64.iso

### OpenStack
- Using `vpn.cmu.edu`
- DevStack [installation](https://docs.openstack.org/devstack/latest/)
- Copy the `local.conf` file from the `samples` directory in the devstack repository with four passwords preset at the root of the devstack git repo. Besides, you can also preset the host ip and ipv6. The command to get these in 18.04 is `ip a`.

```
# Minimal Contents
# ----------------

# While ``stack.sh`` is happy to run without ``localrc``, devlife is better when
# there are a few minimal variables set:

# If the ``*_PASSWORD`` variables are not set here you will be prompted to enter
# values for them by ``stack.sh``and they will be added to ``local.conf``.
ADMIN_PASSWORD=nomoresecret
DATABASE_PASSWORD=stackdb
RABBIT_PASSWORD=stackqueue
SERVICE_PASSWORD=$ADMIN_PASSWORD

# ``HOST_IP`` and ``HOST_IPV6`` should be set manually for best results if
# the NIC configuration of the host is unusual, i.e. ``eth1`` has the default
# route but ``eth0`` is the public interface.  They are auto-detected in
# ``stack.sh`` but often is indeterminate on later runs due to the IP moving
# from an Ethernet interface to a bridge on the host. Setting it here also
# makes it available for ``openrc`` to include when setting ``OS_AUTH_URL``.
# Neither is set by default.
#HOST_IP=w.x.y.z
#HOST_IPV6=2001:db8::7
```
